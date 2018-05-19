#include <defs.h>
#include <string.h>
#include <stdlib.h>
#include <list.h>
#include <stat.h>
//#include <kmalloc.h>
#include <slab.h>
#include <vfs.h>
#include <dev.h>
#include <sfs.h>
#include <inode.h>
#include <iobuf.h>
#include <bitmap.h>
#include <error.h>
#include <assert.h>
#include <stdio.h>

//目录对应的操作函数集合
static const struct inode_ops sfs_node_dirops;  // dir operations
//文件对应的操作函数集合
static const struct inode_ops sfs_node_fileops; // file operations

static inline int
trylock_sin(struct sfs_inode *sin) {
    if (!SFSInodeRemoved(sin)) {
        down(&(sin->sem));
        if (!SFSInodeRemoved(sin)) {
            return 0;
        }
        up(&(sin->sem));
    }
    return -E_NOENT;
}

/*
 * lock_sin - lock the process of inode Rd/Wr
 */
static void
lock_sin(struct sfs_inode *sin) {
    down(&(sin->sem));
}

/*
 * unlock_sin - unlock the process of inode Rd/Wr
 */
static void
unlock_sin(struct sfs_inode *sin) {
    up(&(sin->sem));
}

/*
 * sfs_get_ops - return function addr of fs_node_dirops/sfs_node_fileops
 */
static const struct inode_ops *
sfs_get_ops(uint16_t type) {
    switch (type) {
    case SFS_TYPE_DIR:
        return &sfs_node_dirops;
    case SFS_TYPE_FILE:
        return &sfs_node_fileops;
    }
    panic("invalid file type %d.\n", type);
}

/*
 * sfs_hash_list - return inode entry in sfs->hash_list
 * 根据 ino从sfs->hash_list中得到inode
 */
static list_entry_t *
sfs_hash_list(struct sfs_fs *sfs, uint32_t ino) {
    return sfs->hash_list + sin_hashfn(ino);
}

/*
 * sfs_set_links - link inode sin in sfs->linked-list AND sfs->hash_link
 * 添加inode到 sfs->hash-list 和 sfs->inode_list中去
 *
 *  sfs_load_inode
 *      sfs_set_links
 */
static void
sfs_set_links(struct sfs_fs *sfs, struct sfs_inode *sin) {
    list_add(&(sfs->inode_list), &(sin->inode_link));
    list_add(sfs_hash_list(sfs, sin->ino), &(sin->hash_link));
}

/*
 * sfs_remove_links - unlink inode sin in sfs->linked-list AND sfs->hash_link
 * 从sfs->hash-list 和 sfs->inode_list中删除 inode
 */
static void
sfs_remove_links(struct sfs_inode *sin) {
    list_del(&(sin->inode_link));
    list_del(&(sin->hash_link));
}

/*
 * sfs_block_inuse - check the inode with NO. ino inuse info in bitmap
 * 判断freemap[ino]是否已经被使用
 */
static bool
sfs_block_inuse(struct sfs_fs *sfs, uint32_t ino) {
    if (ino != 0 && ino < sfs->super.blocks) {
        return !bitmap_test(sfs->freemap, ino);
    }
    panic("sfs_block_inuse: called out of range (0, %u) %u.\n", sfs->super.blocks, ino);
}

/*
 * sfs_block_alloc -  check and get a free disk block
 */
static int
sfs_block_alloc(struct sfs_fs *sfs, uint32_t *ino_store) {
    int ret;
    if ((ret = bitmap_alloc(sfs->freemap, ino_store)) != 0) {
        //失败
        return ret;
    }
    assert(sfs->super.unused_blocks > 0);
    sfs->super.unused_blocks --, sfs->super_dirty = 1;
    assert(sfs_block_inuse(sfs, *ino_store));
    //清空block上旧的数据
    return sfs_clear_block(sfs, *ino_store, 1);
}

/*
 * sfs_block_free - set related bits for ino block to 1(means free) in bitmap, add sfs->super.unused_blocks, set superblock dirty *
 * 退还block，设置sfs->freemap[ino]=0
 */
static void
sfs_block_free(struct sfs_fs *sfs, uint32_t ino) {
    assert(sfs_block_inuse(sfs, ino));
    bitmap_free(sfs->freemap, ino);
    sfs->super.unused_blocks ++, sfs->super_dirty = 1;
}

/*
 * sfs_create_inode - alloc a inode in memroy, and init din/ino/dirty/reclian_count/sem fields in sfs_inode in inode
 * 创建 inode,其中inode->type = inode_type_sfs_inode_info
 *
 * sfs_get_root 或
 * sfs_lookup_once
 *      sfs_load_inode
 *          sfs_create_inode
 */
static int
sfs_create_inode(struct sfs_fs *sfs, struct sfs_disk_inode *din, uint32_t ino, struct inode **node_store) {
    struct inode *node;
    ////   node = __alloc_inode(__sfs_inode_info) ;
    if ((node = alloc_inode(sfs_inode)) != NULL) {
        //inode_init(node, sfs_node_dirops |sfs_node_fileops ,sfs->fs_info->__sfs_info);
        vop_init(node, sfs_get_ops(din->type), info2fs(sfs, sfs));
        
        //sin=node->in_info->__sfs_inode_info
        struct sfs_inode *sin = vop_info(node, sfs_inode);

        sin->din = din, sin->ino = ino, sin->dirty = 0, sin->flags = 0, sin->reclaim_count = 1;
        sem_init(&(sin->sem), 1);
        *node_store = node;
        return 0;
    }
    return -E_NO_MEM;
}

/*
 * lookup_sfs_nolock - according ino, find related inode
 *
 * NOTICE: le2sin, info2node MACRO
 * 根据ino, 从sfs->hash_link中查找 inode
 */
static struct inode *
lookup_sfs_nolock(struct sfs_fs *sfs, uint32_t ino) {
    struct inode *node;
    //hash表头
    list_entry_t *list = sfs_hash_list(sfs, ino), *le = list;
    
    while ((le = list_next(le)) != list) {
        // 得到sfs_inode
        struct sfs_inode *sin = le2sin(le, hash_link);
        if (sin->ino == ino) {
            //对上，找到返回 inode
            node = info2node(sin, sfs_inode);
            if (vop_ref_inc(node) == 1) {
                sin->reclaim_count ++;
            }
            return node;
        }
    }
    return NULL;
}

/*
 * sfs_load_inode - If the inode isn't existed, load inode related ino disk block data into a new created inode.
 *                  If the inode is in memory alreadily, then do nothing
 *
 * 根据ino，读取sfs_disk_inode,并且建立inode
 */
int
sfs_load_inode(struct sfs_fs *sfs, struct inode **node_store, uint32_t ino) {
    lock_sfs_fs(sfs);
    struct inode *node;
    //先从sfs->hash_link中查找ino相关的inode对象
    if ((node = lookup_sfs_nolock(sfs, ino)) != NULL) {
        //成功
        goto out_unlock;
    }

    int ret = -E_NO_MEM;
    struct sfs_disk_inode *din;
    //分配sfs_disk_inode,准备从磁盘读取，填空din
    if ((din = kmalloc(sizeof(struct sfs_disk_inode))) == NULL) {
        goto failed_unlock;
    }

    assert(sfs_block_inuse(sfs, ino));
    //读取第ino的block到din中
    if ((ret = sfs_rbuf(sfs, din, sizeof(struct sfs_disk_inode), ino, 0)) != 0) {
        goto failed_cleanup_din;
    }
    /*
    cprintf("---------------------------------------\n");
    if (din->type == SFS_TYPE_FILE) {
        cprintf(" file size : %d\n", din->fileinfo.size);
    } else if (din->type == SFS_TYPE_DIR){
        cprintf("dir:\n");
        cprintf("  slots: %d\n",din->dirinfo.slots);
        cprintf("  parent: %d\n",din->dirinfo.parent);
    } else {
        cprintf("unknown:\n");
    }
    cprintf("ino = %d\n",ino);
    cprintf("nlinks = %d\n",din->nlinks);
    cprintf("type = %d \n",din->type);
    cprintf("blocks = %d \n",din->blocks);
    cprintf("---------------------------------------\n");
    */
    assert(din->nlinks != 0);
    //将sfs_disk_inode转成inode
    if ((ret = sfs_create_inode(sfs, din, ino, &node)) != 0) {
        cprintf(" sfs_create_inode failed ret= %d\n",ret);
        goto failed_cleanup_din;
    }
    
    //node->in_info.__sfs_inode_info
    //将node对象放到 sfs->hash_link和sfs->inode_list中
    sfs_set_links(sfs, vop_info(node, sfs_inode));

out_unlock:
    unlock_sfs_fs(sfs);
    *node_store = node;
    return 0;

failed_cleanup_din:
    kfree(din);
failed_unlock:
    unlock_sfs_fs(sfs);
    return ret;
}

/*
 * sfs_bmap_get_sub_nolock - according entry pointer entp and index, find the index of indrect disk block
 *                           return the index of indrect disk block to ino_store. no lock protect
 * @sfs:      sfs file system
 * @entp:     the pointer of index of entry disk block
 * @index:    the index of block in indrect block
 * @create:   BOOL, if the block isn't allocated, if create = 1 the alloc a block,  otherwise just do nothing
 * @ino_store: 0 OR the index of already inused block or new allocated block.
 *
 *
 * 从 indirect中查找 index 的block id
 * sysfile_getdirentry
 *    file_getdirentry
 *       sfs_getdirentry(sfs_node_dirops->vop_getdirentry)
 *          sfs_getdirentry_sub_nolock
 *                sfs_dirent_read_nolock
 *                      sfs_bmap_load_nolock
 *                           sfs_bmap_get_nolock
 *                               sfs_bmap_get_sub_nolock
 *
 */
static int
sfs_bmap_get_sub_nolock(struct sfs_fs *sfs, uint32_t *entp, uint32_t index, bool create, uint32_t *ino_store) {
    assert(index < SFS_BLK_NENTRY);
    int ret;
    uint32_t ent, ino = 0;
    //这个offset 是 block上的offset了
    off_t offset = index * sizeof(uint32_t);  // the offset of entry in entry block
	// if entry block is existd, read the content of entry block into  sfs->sfs_buffer
    if ((ent = *entp) != 0) {
        if ((ret = sfs_rbuf(sfs, &ino, sizeof(uint32_t), ent, offset)) != 0) {
            return ret;
        }
        if (ino != 0 || !create) {
            goto out;
        }
    }
    else {
        if (!create) {
            goto out;
        }
		//if entry block isn't existd, allocated a entry block (for indrect block)
        if ((ret = sfs_block_alloc(sfs, &ent)) != 0) {
            return ret;
        }
    }
    //重新分配 ino
    if ((ret = sfs_block_alloc(sfs, &ino)) != 0) {
        goto failed_cleanup;
    }
    if ((ret = sfs_wbuf(sfs, &ino, sizeof(uint32_t), ent, offset)) != 0) {
        sfs_block_free(sfs, ino);
        goto failed_cleanup;
    }

out:
    if (ent != *entp) {
        *entp = ent;
    }
    *ino_store = ino;
    return 0;

failed_cleanup:
    if (ent != *entp) {
        sfs_block_free(sfs, ent);
    }
    return ret;
}

/*
 * sfs_bmap_get_nolock - according sfs_inode and index of block, find the NO. of disk block
 *                       no lock protect
 * @sfs:      sfs file system
 * @sin:      sfs inode in memory
 * @index:    the index of block in inode
 * @create:   BOOL, if the block isn't allocated, if create = 1 the alloc a block,  otherwise just do nothing
 * @ino_store: 0 OR the index of already inused block or new allocated block.
 *
 *
 *
 * sfs_io_nolock
 * sfs_truncfile
 *   sfs_bmap_load_nolock
 *       sfs_bmap_get_nolock
 *
 *
 *
 * sysfile_getdirentry
 *    file_getdirentry
 *       sfs_getdirentry(sfs_node_dirops->vop_getdirentry)
 *          sfs_getdirentry_sub_nolock
 *                sfs_dirent_read_nolock
 *                      sfs_bmap_load_nolock
 *                            sfs_bmap_get_nolock
 *
 *
 * 从 sfs_disk_inode中获取 第index个
 */
static int
sfs_bmap_get_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, uint32_t index, bool create, uint32_t *ino_store) {
    //得到具体的 sfs_disk_inode
    struct sfs_disk_inode *din = sin->din;
    int ret;
    uint32_t ent, ino;
	// the index of disk block is in the fist SFS_NDIRECT  direct blocks
    
    if (index < SFS_NDIRECT) {
        //在sfs_disk_inode->direct[]中就可以确定
        if ((ino = din->direct[index]) == 0 && create) {
            //sfs->freemap 中分配 一个block,block id 保存到ino
            if ((ret = sfs_block_alloc(sfs, &ino)) != 0) {
                //失败
                return ret;
            }
            //block id 填入
            din->direct[index] = ino;
            sin->dirty = 1;
        }
        goto out;
    }
    // the index of disk block is in the indirect blocks.
    //从 sfs->din->indirect中读取，索引要先减掉 direct block的长度
    index -= SFS_NDIRECT;
    //索引必须小于block中entry的数量
    if (index < SFS_BLK_NENTRY) {
        //从sfs_disk_inode->indirect中确定
        ent = din->indirect;
        if ((ret = sfs_bmap_get_sub_nolock(sfs, &ent, index, create, &ino)) != 0) {
            return ret;
        }
        if (ent != din->indirect) {
            assert(din->indirect == 0);
            din->indirect = ent;
            sin->dirty = 1;
        }
        goto out;
    } else {
		panic ("sfs_bmap_get_nolock - index out of range");
	}
out:
    assert(ino == 0 || sfs_block_inuse(sfs, ino));
    *ino_store = ino;
    return 0;
}

/*
 * sfs_bmap_free_sub_nolock - set the entry item to 0 (free) in the indirect block
 */
static int
sfs_bmap_free_sub_nolock(struct sfs_fs *sfs, uint32_t ent, uint32_t index) {
    assert(sfs_block_inuse(sfs, ent) && index < SFS_BLK_NENTRY);
    int ret;
    uint32_t ino, zero = 0;
    off_t offset = index * sizeof(uint32_t);
    if ((ret = sfs_rbuf(sfs, &ino, sizeof(uint32_t), ent, offset)) != 0) {
        return ret;
    }
    if (ino != 0) {
        if ((ret = sfs_wbuf(sfs, &zero, sizeof(uint32_t), ent, offset)) != 0) {
            return ret;
        }
        sfs_block_free(sfs, ino);
    }
    return 0;
}

/*
 * sfs_bmap_free_nolock - free a block with logical index in inode and reset the inode's fields
 */
static int
sfs_bmap_free_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, uint32_t index) {
    struct sfs_disk_inode *din = sin->din;
    int ret;
    uint32_t ent, ino;
    if (index < SFS_NDIRECT) {
        if ((ino = din->direct[index]) != 0) {
			// free the block
            sfs_block_free(sfs, ino);
            din->direct[index] = 0;
            sin->dirty = 1;
        }
        return 0;
    }

    index -= SFS_NDIRECT;
    if (index < SFS_BLK_NENTRY) {
        if ((ent = din->indirect) != 0) {
			// set the entry item to 0 in the indirect block
            if ((ret = sfs_bmap_free_sub_nolock(sfs, ent, index)) != 0) {
                return ret;
            }
        }
        return 0;
    }
    return 0;
}

/*
 * sfs_bmap_load_nolock - according to the DIR's inode and the logical index of block in inode, find the NO. of disk block.
 * @sfs:      sfs file system
 * @sin:      sfs inode in memory
 * @index:    the logical index of disk block in inode
 * @ino_store:the NO. of disk block
 *
 *
 *
 * sfs_io_nolock
 * sfs_truncfile
 *      sfs_bmap_load_nolock
 *
 *
 *
 * sysfile_getdirentry
 *  file_getdirentry
 *       sfs_getdirentry(sfs_node_dirops->vop_getdirentry)
 *          sfs_getdirentry_sub_nolock
 *                sfs_dirent_read_nolock
 *                      sfs_bmap_load_nolock
 *
 *
 * 将对应sfs_inode的第index个索引指向的block的索引值取出存到相应的指针指向的单元(ino_store)
 */
static int
sfs_bmap_load_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, uint32_t index, uint32_t *ino_store) {
    /*
     * 得到sfs_inode 对应的磁盘 sfs_disk_inode
     * sfs_disk_inode->direct 包含相应的 sfs_disk_entry
     */
    struct sfs_disk_inode *din = sin->din;
    /*
     * 只接受 index <= din->blocks的状况，当index == din->blocks时
     * 本函数理解为需要inode增长一个block，并且标记inode为dirty
     */
    assert(index <= din->blocks);
    int ret;
    uint32_t ino;
    //需要为inode增长一个block
    bool create = (index == din->blocks);
    if ((ret = sfs_bmap_get_nolock(sfs, sin, index, create, &ino)) != 0) {
        return ret;
    }
    assert(sfs_block_inuse(sfs, ino));
    if (create) {
        din->blocks ++;
    }
    if (ino_store != NULL) {
        *ino_store = ino;
    }
    return 0;
}

/*
 * sfs_bmap_truncate_nolock - free the disk block at the end of file
 *
 * 将多级数据索引表的最后一个entry释放掉，
 * 可以认为是sfs_bmap_load_nolock(index==inode->blocks时)的逆操做
 * 当一个文件或者目录被删除时，sfs会循环调用本函数,直到inode->blocks减为0，释放所有的数据页
 * 
 */
static int
sfs_bmap_truncate_nolock(struct sfs_fs *sfs, struct sfs_inode *sin) {
    struct sfs_disk_inode *din = sin->din;
    assert(din->blocks != 0);
    int ret;
    if ((ret = sfs_bmap_free_nolock(sfs, sin, din->blocks - 1)) != 0) {
        return ret;
    }
    din->blocks --;
    sin->dirty = 1;
    return 0;
}

/*
 * sfs_dirent_read_nolock - read the file entry from disk block which contains this entry
 * @sfs:      sfs file system
 * @sin:      sfs inode in memory
 * @slot:     the index of file entry
 * @entry:    file entry
 *
 *
 * sfs_dirent_findino_nolock 
 * sfs_dirent_search_nolock
 *     sfs_dirent_read_nolock
 *
 *
 *sysfile_getdirentry
 *  file_getdirentry
 *       sfs_getdirentry(sfs_node_dirops->vop_getdirentry)
 *          sfs_getdirentry_sub_nolock
 *                sfs_dirent_read_nolock
 *
 * 将目录的第slot个entry读取到指定的内存空间(entry)。
 */
static int
sfs_dirent_read_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, int slot, struct sfs_disk_entry *entry) {
    //必须是目录类型的sfs_inode
    assert(sin->din->type == SFS_TYPE_DIR && (slot >= 0 && slot < sin->din->blocks));
    int ret;
    uint32_t ino;
	// according to the DIR's inode and the slot of file entry, find the index of disk block which contains this file entry
    /*
     * sin->din->type== SFS_TYPE_DIR
     * 读取相应目录下的第slot个sfs_disk_entry
     * 并且根据sfs_disk_entry到ino
     *
     */
    if ((ret = sfs_bmap_load_nolock(sfs, sin, slot, &ino)) != 0) {
        return ret;
    }
    assert(sfs_block_inuse(sfs, ino));
	// read the content of file entry in the disk block 
    if ((ret = sfs_rbuf(sfs, entry, sizeof(struct sfs_disk_entry), ino, 0)) != 0) {
        return ret;
    }
    entry->name[SFS_MAX_FNAME_LEN] = '\0';
    return 0;
}


static int
sfs_dirent_write_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, int slot, uint32_t ino, const char *name) {
    assert(sin->din->type == SFS_TYPE_DIR && (slot >= 0 && slot <= sin->din->blocks));
    struct sfs_disk_entry *entry;
    if ((entry = kmalloc(sizeof(struct sfs_disk_entry))) == NULL) {
        return -E_NO_MEM;
    }
    memset(entry, 0, sizeof(struct sfs_disk_entry));

    if (ino != 0) {
        assert(strlen(name) <= SFS_MAX_FNAME_LEN);
        entry->ino = ino, strcpy(entry->name, name);
    }
    int ret;
    if ((ret = sfs_bmap_load_nolock(sfs, sin, slot, &ino)) != 0) {
        goto out;
    }
    assert(sfs_block_inuse(sfs, ino));
    ret = sfs_wbuf(sfs, entry, sizeof(struct sfs_disk_entry), ino, 0);
out:    
    kfree(entry);
    return ret;
}

static int
sfs_dirent_link_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, int slot, struct sfs_inode *lnksin, const char *name) {
    int ret;
    if ((ret = sfs_dirent_write_nolock(sfs, sin, slot, lnksin->ino, name)) != 0) {
        return ret;
    }
    sin->dirty = 1;
    sin->din->dirinfo.slots ++;
    lnksin->dirty = 1;
    lnksin->din->nlinks ++;
    return 0;
}



#define sfs_dirent_link_nolock_check(sfs, sin, slot, lnksin, name)                  \
    do {                                                                            \
        int err;                                                                    \
        if ((err = sfs_dirent_link_nolock(sfs, sin, slot, lnksin, name)) != 0) {    \
            warn("sfs_dirent_link error: %e.\n", err);                              \
        }                                                                           \
    } while (0)

#define sfs_dirent_unlink_nolock_check(sfs, sin, slot, lnksin)                      \
    do {                                                                            \
        int err;                                                                    \
        if ((err = sfs_dirent_unlink_nolock(sfs, sin, slot, lnksin)) != 0) {        \
            warn("sfs_dirent_unlink error: %e.\n", err);                            \
        }                                                                           \
    } while (0)

/*
 * sfs_dirent_search_nolock - read every file entry in the DIR, compare file name with each entry->name
 *                            If equal, then return slot and NO. of disk of this file's inode
 * @sfs:        sfs file system
 * @sin:        sfs inode in memory
 * @name:       the filename
 * @ino_store:  NO. of disk of this file (with the filename)'s inode
 * @slot:       logical index of file entry (NOTICE: each file entry ocupied one  disk block)
 * @empty_slot: the empty logical index of file entry.
 *
 * 在目录sin下查找name，并且返回搜索结果(文件或者文件夹)的inode编号,
 * 以及相应的entry在该目录的index编号和目录下的数据页是否有空闲的entry
 * 
 */
static int
sfs_dirent_search_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, const char *name, uint32_t *ino_store, int *slot, int *empty_slot) {
    assert(strlen(name) <= SFS_MAX_FNAME_LEN);
    struct sfs_disk_entry *entry;
    if ((entry = kmalloc(sizeof(struct sfs_disk_entry))) == NULL) {
        return -E_NO_MEM;
    }

#define set_pvalue(x, v)            do { if ((x) != NULL) { *(x) = (v); } } while (0)
    // disk_inode 上的 entry数量
    int ret, i, nslots = sin->din->blocks;
    // *empty_slot = nslots
    set_pvalue(empty_slot, nslots);
    for (i = 0; i < nslots; i ++) {
        // 读出第i个entry，保存到entry中
        if ((ret = sfs_dirent_read_nolock(sfs, sin, i, entry)) != 0) {
            goto out;
        }
        if (entry->ino == 0) {
            // *empty_slot = i
            set_pvalue(empty_slot, i);
            continue ;
        }
        if (strcmp(name, entry->name) == 0) {
            //找到
            set_pvalue(slot, i);
            set_pvalue(ino_store, entry->ino);
            goto out;
        }
    }
#undef set_pvalue
    ret = -E_NOENT;
out:
    kfree(entry);
    return ret;
}

/*
 * sfs_dirent_findino_nolock - read all file entries in DIR's inode and find a entry->ino == ino
 */

static int
sfs_dirent_findino_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, uint32_t ino, struct sfs_disk_entry *entry) {
    int ret, i, nslots = sin->din->blocks;
    for (i = 0; i < nslots; i ++) {
        if ((ret = sfs_dirent_read_nolock(sfs, sin, i, entry)) != 0) {
            return ret;
        }
        if (entry->ino == ino) {
            return 0;
        }
    }
    return -E_NOENT;
}

static int
sfs_dirent_create_inode(struct sfs_fs *sfs, uint16_t type, struct inode **node_store) {
    struct sfs_disk_inode *din;
    if ((din = kmalloc(sizeof(struct sfs_disk_inode))) == NULL) {
        return -E_NO_MEM;
    }
    memset(din, 0, sizeof(struct sfs_disk_inode));
    din->type = type;

    int ret;
    uint32_t ino;
    if ((ret = sfs_block_alloc(sfs, &ino)) != 0) {
        goto failed_cleanup_din;
    }
    struct inode *node;
    if ((ret = sfs_create_inode(sfs, din, ino, &node)) != 0) {
        goto failed_cleanup_ino;
    }
    lock_sfs_fs(sfs);
    {
        sfs_set_links(sfs, vop_info(node, sfs_inode));
    }
    unlock_sfs_fs(sfs);
    *node_store = node;
    return 0;
failed_cleanup_ino:
    sfs_block_free(sfs, ino);
failed_cleanup_din:
    kfree(din);
    return ret;    
}

static int
sfs_load_parent(struct sfs_fs *sfs, struct sfs_inode *sin, struct inode **parent_store) {
    return sfs_load_inode(sfs, parent_store, sin->din->dirinfo.parent);
}

/*
 * sfs_lookup_once - find inode corresponding the file name in DIR's sin inode 
 * @sfs:        sfs file system
 * @sin:        DIR sfs inode in memory
 * @name:       the file name in DIR
 * @node_store: the inode corresponding the file name in DIR
 * @slot:       the logical index of file entry
 *
 * sfs_namefile
 *      sfs_lookup_once
 *
 * sfs_lookup
 *      sfs_lookup_once
 */
static int
sfs_lookup_once(struct sfs_fs *sfs, struct sfs_inode *sin, const char *name, struct inode **node_store, int *slot) {
    int ret;
    uint32_t ino;
    lock_sin(sin);
    {   // find the NO. of disk block and logical index of file entry
        ret = sfs_dirent_search_nolock(sfs, sin, name, &ino, slot, NULL);
    }
    unlock_sin(sin);
    if (ret == 0) {
        //上面的成功返回
		// load the content of inode with the the NO. of disk block
        ret = sfs_load_inode(sfs, node_store, ino);
    }
    return ret;
}

/*****************************************************************************************************************
上面的都是工具函数
下面的大多是 inode_ops函数 
*****************************************************************************************************************/
// sfs_opendir - just check the opne_flags, now support readonly
// sfs_node_dirops->vop_open
// 目录打开，都成功
/*
 * sys_open->
 *   sysfile_open
 *      file_open
 *          vfs_open
 *              sfs_opendir
 */
static int
sfs_opendir(struct inode *node, uint32_t open_flags, uint32_t arg) {
    switch (open_flags & O_ACCMODE) {
    case O_RDONLY:
        break;
    case O_WRONLY:
    case O_RDWR:
    default:
        return -E_ISDIR;
    }
    if (open_flags & O_APPEND) {
        return -E_ISDIR;
    }
    return 0;
}

// sfs_openfile - open file (no use)
// sfs_node_fileops->vop_open
// 打开文件都成功
/*
 * 
 * sys_open->
 *   sysfile_open
 *      file_open
 *          vfs_open
 *              sfs_openfile
 */
static int
sfs_openfile(struct inode *node, uint32_t open_flags,uint32_t arg) {
    return 0;
}

// sfs_close - close file
// sfs_node_fileops->vop_close
// sfs_node_dirops->vop_close
static int
sfs_close(struct inode *node) {
    
    //关闭文件数据同步
    // sfs_close(node)
    return vop_fsync(node);
}

/*  
 * sfs_io_nolock - Rd/Wr a file contentfrom offset position to offset+ length  disk blocks<-->buffer (in memroy)
 * @sfs:      sfs file system
 * @sin:      sfs inode in memory
 * @buf:      the buffer Rd/Wr
 * @offset:   the offset of file
 * @alenp:    the length need to read (is a pointer). and will RETURN the really Rd/Wr length
 * @write:    BOOL, 0 read, 1 write
 *
 *
 * sys_read
 *    sysfile_read
 *         file_read
 *            sfs_read   
 *               sfs_io
 *                  sfs_io_nolock
 */
static int
sfs_io_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, void *buf, off_t offset, size_t *alenp, bool write) {
    struct sfs_disk_inode *din = sin->din;
    assert(din->type != SFS_TYPE_DIR);
    off_t endpos = offset + *alenp/*结束点*/, blkoff;
    *alenp = 0;
	// calculate the Rd/Wr end position
    if (offset < 0 || offset >= SFS_MAX_FILE_SIZE || offset > endpos) {
        return -E_INVAL;
    }
    if (offset == endpos) {
        return 0;
    }
    if (endpos > SFS_MAX_FILE_SIZE) {
        //不能超过最大的文件长度
        endpos = SFS_MAX_FILE_SIZE;
    }
    if (!write) {
        if (offset >= din->fileinfo.size) {
            return 0;
        }
        if (endpos > din->fileinfo.size) {
            //读，不能超过文件的大小
           endpos = din->fileinfo.size;
        }
    }

    int (*sfs_buf_op)(struct sfs_fs *sfs, void *buf, size_t len, uint32_t blkno, off_t offset);
    int (*sfs_block_op)(struct sfs_fs *sfs, void *buf, uint32_t blkno, uint32_t nblks);
    if (write) {
        //写
        sfs_buf_op = sfs_wbuf, sfs_block_op = sfs_wblock;
    }
    else {
        //读
        sfs_buf_op = sfs_rbuf, sfs_block_op = sfs_rblock;
    }

    int ret = 0;
    size_t size, alen = 0;
    uint32_t ino;
    uint32_t blkno = offset / SFS_BLKSIZE;          // The NO. of Rd/Wr begin block
    uint32_t nblks = endpos / SFS_BLKSIZE - blkno;  // The size of Rd/Wr blocks

  	/*
	 * (1) If offset isn't aligned with the first block, Rd/Wr some content from offset to the end of the first block
	 *       NOTICE: useful function: sfs_bmap_load_nolock, sfs_buf_op
	 *               Rd/Wr size = (nblks != 0) ? (SFS_BLKSIZE - blkoff) : (endpos - offset)
	 * (2) Rd/Wr aligned blocks 
	 *       NOTICE: useful function: sfs_bmap_load_nolock, sfs_block_op
     * (3) If end position isn't aligned with the last block, Rd/Wr some content from begin to the (endpos % SFS_BLKSIZE) of the last block
	 *       NOTICE: useful function: sfs_bmap_load_nolock, sfs_buf_op	
	*/
    if ((blkoff = offset % SFS_BLKSIZE) != 0) {
        size = (nblks != 0) ? (SFS_BLKSIZE - blkoff) : (endpos - offset);
        if ((ret = sfs_bmap_load_nolock(sfs, sin, blkno, &ino)) != 0) {
            goto out;
        }
        if ((ret = sfs_buf_op(sfs, buf, size, ino, blkoff)) != 0) {
            goto out;
        }
        alen += size;
        if (nblks == 0) {
            goto out;
        }
        buf += size, blkno ++, nblks --;
    }

    size = SFS_BLKSIZE;
    while (nblks != 0) {
        if ((ret = sfs_bmap_load_nolock(sfs, sin, blkno, &ino)) != 0) {
            goto out;
        }
        if ((ret = sfs_block_op(sfs, buf, ino, 1)) != 0) {
            goto out;
        }
        alen += size, buf += size, blkno ++, nblks --;
    }

    if ((size = endpos % SFS_BLKSIZE) != 0) {
        if ((ret = sfs_bmap_load_nolock(sfs, sin, blkno, &ino)) != 0) {
            goto out;
        }
        if ((ret = sfs_buf_op(sfs, buf, size, ino, 0)) != 0) {
            goto out;
        }
        alen += size;
    }
out:
    *alenp = alen;
    if (offset + alen > sin->din->fileinfo.size) {
        sin->din->fileinfo.size = offset + alen;
        sin->dirty = 1;
    }
    return ret;
}

/*
 * sfs_io - Rd/Wr file. the wrapper of sfs_io_nolock
            with lock protect
 * sys_read
 *    sysfile_read
 *         file_read
 *            sfs_read   
 *               sfs_io
 */
static inline int
sfs_io(struct inode *node, struct iobuf *iob, bool write) {
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    struct sfs_inode *sin = vop_info(node, sfs_inode);
    int ret;
    lock_sin(sin);
    {
        size_t alen = iob->io_resid;
        ret = sfs_io_nolock(sfs, sin, iob->io_base, iob->io_offset, &alen, write);
        if (alen != 0) {
            iobuf_skip(iob, alen);
        }
    }
    unlock_sin(sin);
    return ret;
}

// sfs_read - read file
// sfs_node_fileops->vop_read
/*
 * sys_read
 *    sysfile_read
 *         file_read
 *            sfs_read
 */
static int
sfs_read(struct inode *node, struct iobuf *iob) {
    return sfs_io(node, iob, 0);
}

// sfs_write - write file
// sfs_node_fileops->vop_write
static int
sfs_write(struct inode *node, struct iobuf *iob) {
    return sfs_io(node, iob, 1);
}



/*
 * sfs_fstat - Return nlinks/block/size, etc. info about a file. The pointer is a pointer to struct stat;
 *
 * sfs_node_fileops->vop_fstat
 * sfs_node_dirops->vop_fsta
 */
static int
sfs_fstat(struct inode *node, struct stat *stat) {
    int ret;
    memset(stat, 0, sizeof(struct stat));
    //node->in_ops->vop_gettype(node, &(stat->st_mode))
    if ((ret = vop_gettype(node, &(stat->st_mode))) != 0) {
        return ret;
    }
    //din=node->in_info.__sfs_inode_info
    struct sfs_disk_inode *din = vop_info(node, sfs_inode)->din;
    stat->st_nlinks = din->nlinks;
    stat->st_blocks = din->blocks;
    if (din->type != SFS_TYPE_DIR) {
        stat->st_size = din->fileinfo.size;
    }
    else {
        stat->st_size = (din->dirinfo.slots + 2) * sfs_dentry_size;
    }
    return 0;
}

static inline void
sfs_nlinks_inc_nolock(struct sfs_inode *sin) {
    sin->dirty = 1, ++ sin->din->nlinks;
}

static inline void
sfs_nlinks_dec_nolock(struct sfs_inode *sin) {
    assert(sin->din->nlinks != 0);
    sin->dirty = 1, -- sin->din->nlinks;
}


static inline void
sfs_dirinfo_set_parent(struct sfs_inode *sin, struct sfs_inode *parent) {
    sin->dirty = 1;
    sin->din->dirinfo.parent = parent->ino;
}

/*
 * sfs_fsync - Force any dirty inode info associated with this file to stable storage.
 *
 * sfs_node_fileops->fsync
 * sfs_node_dirops->fsync
 */
static int
sfs_fsync(struct inode *node) {
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    struct sfs_inode *sin = vop_info(node, sfs_inode);
    int ret = 0;
    if (sin->dirty) {
        lock_sin(sin);
        {
            if (sin->dirty) {
                sin->dirty = 0;
                if ((ret = sfs_wbuf(sfs, sin->din, sizeof(struct sfs_disk_inode), sin->ino, 0)) != 0) {
                    sin->dirty = 1;
                }
            }
        }
        unlock_sin(sin);
    }
    return ret;
}

/*
 *sfs_namefile -Compute pathname relative to filesystem root of the file and copy to the specified io buffer.
 *  
 * sfs_node_dirops->vop_namefile
 */
static int
sfs_namefile(struct inode *node, struct iobuf *iob) {
    struct sfs_disk_entry *entry;
    if (iob->io_resid <= 2 || (entry = kmalloc(sizeof(struct sfs_disk_entry))) == NULL) {
        return -E_NO_MEM;
    }

    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    struct sfs_inode *sin = vop_info(node, sfs_inode);

    int ret;
    char *ptr = iob->io_base + iob->io_resid;
    size_t alen, resid = iob->io_resid - 2;
    vop_ref_inc(node);
    while (1) {
        struct inode *parent;
        if ((ret = sfs_lookup_once(sfs, sin, "..", &parent, NULL)) != 0) {
            goto failed;
        }

        uint32_t ino = sin->ino;
        vop_ref_dec(node);
        if (node == parent) {
            vop_ref_dec(node);
            break;
        }

        node = parent, sin = vop_info(node, sfs_inode);
        assert(ino != sin->ino && sin->din->type == SFS_TYPE_DIR);

        lock_sin(sin);
        {
            ret = sfs_dirent_findino_nolock(sfs, sin, ino, entry);
        }
        unlock_sin(sin);

        if (ret != 0) {
            goto failed;
        }

        if ((alen = strlen(entry->name) + 1) > resid) {
            goto failed_nomem;
        }
        resid -= alen, ptr -= alen;
        memcpy(ptr, entry->name, alen - 1);
        ptr[alen - 1] = '/';
    }
    alen = iob->io_resid - resid - 2;
    ptr = memmove(iob->io_base + 1, ptr, alen);
    ptr[-1] = '/', ptr[alen] = '\0';
    iobuf_skip(iob, alen);
    kfree(entry);
    return 0;

failed_nomem:
    ret = -E_NO_MEM;
failed:
    vop_ref_dec(node);
    kfree(entry);
    return ret;
}


static int
sfs_mkdir_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, const char *name) {
    int ret, slot;
    if ((ret = sfs_dirent_search_nolock(sfs, sin, name, NULL, NULL, &slot)) != -E_NOENT) {
        return (ret != 0) ? ret : -E_EXISTS;
    }
    struct inode *link_node;
    if ((ret = sfs_dirent_create_inode(sfs, SFS_TYPE_DIR, &link_node)) != 0) {
        return ret;
    }
    struct sfs_inode *lnksin = vop_info(link_node, sfs_inode);
    if ((ret = sfs_dirent_link_nolock(sfs, sin, slot, lnksin, name)) != 0) {
        assert(lnksin->din->nlinks == 0);
        assert(inode_ref_count(link_node) == 1 && inode_open_count(link_node) == 0);
        goto out;
    }

    /* set parent */
    sfs_dirinfo_set_parent(lnksin, sin);

    /* add '.' link to itself */
    sfs_nlinks_inc_nolock(lnksin);

    /* add '..' link to parent */
    sfs_nlinks_inc_nolock(sin);

out:
    vop_ref_dec(link_node);
    return ret;
}

static int
sfs_mkdir(struct inode *node, const  char *name) {
    if (strlen(name) > SFS_MAX_FNAME_LEN) {
        return -E_TOO_BIG;
    }
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        return -E_EXISTS;
    }
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    struct sfs_inode *sin = vop_info(node, sfs_inode);
    int ret;
    if ((ret = trylock_sin(sin)) == 0) {
        ret = sfs_mkdir_nolock(sfs, sin, name);
        unlock_sin(sin);
    }
    return ret;
}


/*
 * sfs_getdirentry_sub_noblock - get the content of file entry in DIR
 *
 *
 *sysfile_getdirentry
 *  file_getdirentry
 *       sfs_getdirentry(sfs_node_dirops->vop_getdirentry)
 *          sfs_getdirentry_sub_nolock
 */
static int
sfs_getdirentry_sub_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, int slot, struct sfs_disk_entry *entry) {
    //有nslots个sfs_disk_entry
    int ret, i, nslots = sin->din->blocks;
    
    for (i = 0; i < nslots; i ++) {
        // 读取第i个block
        if ((ret = sfs_dirent_read_nolock(sfs, sin, i, entry)) != 0) {
            return ret;
        }
        
        if (entry->ino != 0) {
            //这样搞，难道是因为排序不一样的，一定有实际的次序，和磁盘上的次序区分的
            if (slot == 0) {
                return 0;
            }
            slot --;
        }
    }
    return -E_NOENT;
}

/*
 * sfs_getdirentry - according to the iob->io_offset, calculate the dir entry's slot in disk block,
                     get dir entry content from the disk 
 *
 * sfs_node_dirops->
 *
 *sysfile_getdirentry
 *   file_getdirentry
 *      sfs_getdirentry(sfs_node_dirops->vop_getdirentry)
 */
static int
sfs_getdirentry(struct inode *node, struct iobuf *iob) {
    struct sfs_disk_entry *entry;
    //读到entry中
    if ((entry = kmalloc(sizeof(struct sfs_disk_entry))) == NULL) {
        return -E_NO_MEM;
    }
    
    // node->in_fs->fs_info.__sfs_info
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    // sin->in_type == SFS_TYPE_DIR
    struct sfs_inode *sin = vop_info(node, sfs_inode);

    int ret, slot;
    //这个offset 就是 dirent->t_offset
    off_t offset = iob->io_offset;
    if (offset < 0 || offset % sfs_dentry_size != 0) {
        //offset偏移不对
        kfree(entry);
        return -E_INVAL;
    }
    // slot 表示第n个block，sin->din->direct[slot] 或者 sin->din->indirect[slot]
    // slot 不能超过 一个文件的block的总数
    // 在sin->in_type == SFS_TYPE_DIR 是，一个sfs_disk_entry一个 block
    if ((slot = offset / sfs_dentry_size) >= sin->din->dirinfo.slots + 2) {
        kfree(entry);
        return -E_NOENT;
    }
    switch(slot) {
    case 0:
         strcpy(entry->name, ".");
         break;
    case 1:
         strcpy(entry->name, "..");
         break;
    default:
         if ((ret = trylock_sin(sin)) != 0) {
            goto out;
        }
        // 读第slot个block(sfs_disk_entry)到entry中去
        ret = sfs_getdirentry_sub_nolock(sfs, sin, slot - 2, entry);
        unlock_sin(sin);
        if (ret != 0) {
            goto out;
        }
    }
    ret = iobuf_move(iob, entry->name, sfs_dentry_size, 1, NULL);
out:
    kfree(entry);
    return ret;
}

/*
 * sfs_reclaim - Free all resources inode occupied . Called when inode is no longer in use. 
 *
 * sfs_node_fileops->vop_reclaim
 * sfs_node_dirops->vop_reclaim
 */
static int
sfs_reclaim(struct inode *node) {
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    struct sfs_inode *sin = vop_info(node, sfs_inode);

    int  ret = -E_BUSY;
    uint32_t ent;
    lock_sfs_fs(sfs);
    assert(sin->reclaim_count > 0);
    if ((-- sin->reclaim_count) != 0 || inode_ref_count(node) != 0) {
        goto failed_unlock;
    }
    if (sin->din->nlinks == 0) {
        if ((ret = vop_truncate(node, 0)) != 0) {
            goto failed_unlock;
        }
    }
    if (sin->dirty) {
        if ((ret = vop_fsync(node)) != 0) {
            goto failed_unlock;
        }
    }
    sfs_remove_links(sin);
    unlock_sfs_fs(sfs);

    if (sin->din->nlinks == 0) {
        sfs_block_free(sfs, sin->ino);
        if ((ent = sin->din->indirect) != 0) {
            sfs_block_free(sfs, ent);
        }
    }
    kfree(sin->din);
    vop_kill(node);
    return 0;

failed_unlock:
    unlock_sfs_fs(sfs);
    return ret;
}

/*
 * sfs_gettype - Return type of file. The values for file types are in sfs.h.
 *
 * sfs_node_fileops->vop_gettype
 * sfs_node_dirops->vop_gettype
 */
static int
sfs_gettype(struct inode *node, uint32_t *type_store) {
    struct sfs_disk_inode *din = vop_info(node, sfs_inode)->din;
    switch (din->type) {
    case SFS_TYPE_DIR:
        *type_store = S_IFDIR;
        return 0;
    case SFS_TYPE_FILE:
        *type_store = S_IFREG;
        return 0;
    case SFS_TYPE_LINK:
        *type_store = S_IFLNK;
        return 0;
    }
    panic("invalid file type %d.\n", din->type);
}

/* 
 * sfs_tryseek - Check if seeking to the specified position within the file is legal.
 *
 * sfs_node_fileops->vop_tryseek
 */
static int
sfs_tryseek(struct inode *node, off_t pos) {
    if (pos < 0 || pos >= SFS_MAX_FILE_SIZE) {
        return -E_INVAL;
    }
    struct sfs_inode *sin = vop_info(node, sfs_inode);
    if (pos > sin->din->fileinfo.size) {
        //sfs_truncfile(node,pos)
        return vop_truncate(node, pos);
    }
    return 0;
}

/*
 * sfs_truncfile : reszie the file with new length
 *
 * sfs_node_fileops->vop_truncate
 *
 * sys_open->
 *   sysfile_open
 *      file_open
 *          vfs_open
 *               sfs_truncfile
 * sfs_tryseek
 * sfs_reclaim
 *       sfs_truncfile
 * 重新设置文件的有效内容、长度
 */
static int
sfs_truncfile(struct inode *node, off_t len) {
    if (len < 0 || len > SFS_MAX_FILE_SIZE) {
        return -E_INVAL;
    }
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    struct sfs_inode *sin = vop_info(node, sfs_inode);
    struct sfs_disk_inode *din = sin->din;

    int ret = 0;
	//new number of disk blocks of file
    uint32_t nblks, tblks = ROUNDUP_DIV(len, SFS_BLKSIZE);
    if (din->fileinfo.size == len) {
        assert(tblks == din->blocks);
        return 0;
    }

    lock_sin(sin);
	// old number of disk blocks of file
    nblks = din->blocks;
    if (nblks < tblks) {
		// try to enlarge the file size by add new disk block at the end of file
        while (nblks != tblks) {
            if ((ret = sfs_bmap_load_nolock(sfs, sin, nblks, NULL)) != 0) {
                goto out_unlock;
            }
            nblks ++;
        }
    }
    else if (tblks < nblks) {
		// try to reduce the file size 
        while (tblks != nblks) {
            if ((ret = sfs_bmap_truncate_nolock(sfs, sin)) != 0) {
                goto out_unlock;
            }
            nblks --;
        }
    }
    assert(din->blocks == tblks);
    din->fileinfo.size = len;
    sin->dirty = 1;

out_unlock:
    unlock_sin(sin);
    return ret;
}


static char *
sfs_lookup_subpath(char *path) {
    if ((path = strchr(path, '/')) != NULL) {
        while (*path == '/') {
            *path ++ = '\0';
        }
        if (*path == '\0') {
            return NULL;
        }
    }
    return path;
}

/*
 * sfs_lookup - Parse path relative to the passed directory
 *              DIR, and hand back the inode for the file it
 *              refers to.
 *
 * sfs_node_dirops->vop_lookup
 *
 *
 */
static int
sfs_lookup(struct inode *node, char *path, struct inode **node_store) {
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    //只能是相对路径
    assert(*path != '\0' && *path != '/');
    vop_ref_inc(node);
    //node->in_info.__sys_inode_info
    struct sfs_inode *sin = vop_info(node, sfs_inode);
    //只能dir类型
    if (sin->din->type != SFS_TYPE_DIR) {
        vop_ref_dec(node);
        return -E_NOTDIR;
    }
    struct inode *subnode;
    int ret = sfs_lookup_once(sfs, sin, path, &subnode, NULL);

    vop_ref_dec(node);
    if (ret != 0) {
        return ret;
    }
    //子路径对应的inode
    *node_store = subnode;
    return 0;
}

static int
sfs_lookup_parent(struct inode *node, char *path, struct inode **node_store, char **endp) {
    struct sfs_fs *sfs = fsop_info(vop_fs(node), sfs);
    assert(*path != '\0' && *path != '/');
    vop_ref_inc(node);
    while (1) {
        struct sfs_inode *sin = vop_info(node, sfs_inode);
        if (sin->din->type != SFS_TYPE_DIR) {
            vop_ref_dec(node);
            return -E_NOTDIR;
        }

        char *subpath;
  next:
        if ((subpath = sfs_lookup_subpath(path)) == NULL) {
            *node_store = node, *endp = path;
            return 0;
        }
        if (strcmp(path, ".") == 0) {
            path = subpath;
            goto next;
        }

        int ret;
        struct inode *subnode;
        if (strcmp(path, "..") == 0) {
            ret = sfs_load_parent(sfs, sin, &subnode);
        }
        else {
            if (strlen(path) > SFS_MAX_FNAME_LEN) {
                vop_ref_dec(node);
                return -E_TOO_BIG;
            }
            ret = sfs_lookup_once(sfs, sin, path, &subnode, NULL);
        }

        vop_ref_dec(node);
        if (ret != 0) {
            return ret;
        }
        node = subnode, path =subpath;
    }
}

// The sfs specific DIR operations correspond to the abstract operations on a inode.
// 所有针对目录的操作
static const struct inode_ops sfs_node_dirops = {
    .vop_magic                      = VOP_MAGIC,
    .vop_open                       = sfs_opendir,
    .vop_close                      = sfs_close,
    .vop_fstat                      = sfs_fstat,
    .vop_fsync                      = sfs_fsync,
    .vop_mkdir                      = sfs_mkdir,
    .vop_namefile                   = sfs_namefile,
    .vop_getdirentry                = sfs_getdirentry,
    .vop_reclaim                    = sfs_reclaim,
    .vop_gettype                    = sfs_gettype,
    .vop_lookup                     = sfs_lookup,
    .vop_lookup_parent              = sfs_lookup_parent,
};
/// The sfs specific FILE operations correspond to the abstract operations on a inode.
// 所有针对文件的操作
static const struct inode_ops sfs_node_fileops = {
    .vop_magic                      = VOP_MAGIC,
    .vop_open                       = sfs_openfile,
    .vop_close                      = sfs_close,
    .vop_read                       = sfs_read,
    .vop_write                      = sfs_write,
    .vop_fstat                      = sfs_fstat,
    .vop_fsync                      = sfs_fsync,
    .vop_mkdir                      = NULL_VOP_NOTDIR,
    .vop_reclaim                    = sfs_reclaim,
    .vop_gettype                    = sfs_gettype,
    .vop_tryseek                    = sfs_tryseek,
    .vop_truncate                   = sfs_truncfile,
    .vop_lookup_parent              = NULL_VOP_NOTDIR,
};

