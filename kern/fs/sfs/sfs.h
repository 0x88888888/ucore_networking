#ifndef __KERN_FS_SFS_SFS_H__
#define __KERN_FS_SFS_SFS_H__

#include <defs.h>
#include <mmu.h>
#include <list.h>
#include <sem.h>
#include <unistd.h>

/*
 * Simple FS (SFS) definitions visible to ucore. This covers the on-disk format
 * and is used by tools that work on SFS volumes, such as mksfs.
 */

#define SFS_MAGIC                                   0x2f8dbe2a              /* magic number for sfs */
#define SFS_BLKSIZE                                 PGSIZE                  /* size of block */
#define SFS_NDIRECT                                 12                      /* # of direct blocks in inode */
#define SFS_MAX_INFO_LEN                            31                      /* max length of infomation */
#define SFS_MAX_FNAME_LEN                           FS_MAX_FNAME_LEN        /* max length of filename */
#define SFS_MAX_FILE_SIZE                           (1024UL * 1024 * 128)   /* max file size (128M) */
#define SFS_BLKN_SUPER                              0                       /* block the superblock lives in */
#define SFS_BLKN_ROOT                               1                       /* location of the root dir inode */
#define SFS_BLKN_FREEMAP                            2                       /* 1st block of the freemap */

/* # of bits in a block */
#define SFS_BLKBITS                                 (SFS_BLKSIZE * CHAR_BIT)

/* # of entries in a block */
#define SFS_BLK_NENTRY                              (SFS_BLKSIZE / sizeof(uint32_t))

/* file types */
#define SFS_TYPE_INVAL                              0       /* Should not appear on disk */
#define SFS_TYPE_FILE                               1
#define SFS_TYPE_DIR                                2
#define SFS_TYPE_LINK                               3

/*
 * On-disk superblock
 */
struct sfs_super {
    uint32_t magic;                                 /* magic number, should be SFS_MAGIC */
    uint32_t blocks;                                /* # of blocks in fs */
    uint32_t unused_blocks;                         /* # of unused blocks in fs */
    char info[SFS_MAX_INFO_LEN + 1];                /* infomation for sfs  */
};

/* inode (on disk) */
struct sfs_disk_inode {
    union {
        struct {
            uint32_t size;                         /* size of the file (in bytes) */
        } fileinfo;
        struct {
            uint32_t slots;                        /* # of entries in this directory */
            uint32_t parent;                       /* parent inode number */
        } dirinfo;
    };
	//uint32_t size;                                  /* 如果inode表示常规文件，则size表示文件的大小 */
    uint16_t type;                                  /* inode代表的是文件(SFS_TYPE_FILE)、目录(SFS_TYPE_DIR)、连接文件(SFS_TYPE_LINK) */
    uint16_t nlinks;                                /* 此inode的硬连接数 */
    /*对于普通文件，索引值指向的 block 中保存的是文件中的数据。
     *而对于目录，
     *索引值指向的数据保存的是目录下所有的文件名以及对应的索引节点所在的索引块(sfs_disk_entry)
     */
    uint32_t blocks;                                /* inode的数据块的个数 */
    uint32_t direct[SFS_NDIRECT];                   /* inode的直接数据块索引值(有SFS_NDIRECT) */
    uint32_t indirect;                              /* inode的一级间接数据块索引值 */
//    uint32_t db_indirect;                           /* double indirect blocks */
//   unused
};

/* file entry (on disk) */
//每个
struct sfs_disk_entry {
	uint32_t ino;                                   /* 文件或者目录的inode number(inode编号) */
    char name[SFS_MAX_FNAME_LEN + 1];               /* 文件或者目录的名称 */
};

#define sfs_dentry_size                             \
    sizeof(((struct sfs_disk_entry *)0)->name)

/* inode for sfs */
// 内存中的inode
struct sfs_inode {
    struct sfs_disk_inode *din;                      /* on-disk inode */
    uint32_t ino;                                    /* inode number */
    uint32_t flags;                                  /* inode flags */
    bool dirty;                                      /* true if inode modified */
    int reclaim_count;                               /* 引用数量 kill inode if it hits zero */
    semaphore_t sem;                                 /* semaphore for din */
    list_entry_t inode_link;                         /* entry for linked-list in sfs_fs */
    list_entry_t hash_link;                          /* entry for hash linked-list in sfs_fs */
};


#define SFS_removed                 0       // the inode has been removed

#define SetSFSInodeRemoved(sin)     set_bit(SFS_removed, &((sin)->flags))
#define ClearSFSInodeRemoved(sin)   clear_bit(SFS_removed, &((sin)->flags))
#define SFSInodeRemoved(sin)        test_bit(SFS_removed, &((sin)->flags))



#define le2sin(le, member)                          \
    to_struct((le), struct sfs_inode, member)

/* filesystem for sfs */
struct sfs_fs {
    struct sfs_super super;                         /* on-disk superblock */
    struct device *dev;                             /* device mounted on */
    struct bitmap *freemap;                         /* blocks in use are mared 0 */
    bool super_dirty;                               /* true if super/freemap modified */
    void *sfs_buffer;                               /* buffer for non-block aligned io */
    semaphore_t fs_sem;                             /* semaphore for fs */
    semaphore_t io_sem;                             /* semaphore for io */
    semaphore_t mutex_sem;                          /* semaphore for link/unlink and rename */
    list_entry_t inode_list;                        /* inode linked-list  */
    list_entry_t *hash_list;                        /* inode hash linked-list */
};

/* hash for sfs */
#define SFS_HLIST_SHIFT                             10
#define SFS_HLIST_SIZE                              (1 << SFS_HLIST_SHIFT)
#define sin_hashfn(x)                               (hash32(x, SFS_HLIST_SHIFT))

/* size of freemap (in bits) */
#define sfs_freemap_bits(super)                     ROUNDUP((super)->blocks, SFS_BLKBITS)

/* size of freemap (in blocks) */
#define sfs_freemap_blocks(super)                   ROUNDUP_DIV((super)->blocks, SFS_BLKBITS)

struct fs;
struct inode;

void sfs_init(void);
int sfs_mount(const char *devname);

void lock_sfs_fs(struct sfs_fs *sfs);
void lock_sfs_io(struct sfs_fs *sfs);
void unlock_sfs_fs(struct sfs_fs *sfs);
void unlock_sfs_io(struct sfs_fs *sfs);

int sfs_rblock(struct sfs_fs *sfs, void *buf, uint32_t blkno, uint32_t nblks);
int sfs_wblock(struct sfs_fs *sfs, void *buf, uint32_t blkno, uint32_t nblks);
int sfs_rbuf(struct sfs_fs *sfs, void *buf, size_t len, uint32_t blkno, off_t offset);
int sfs_wbuf(struct sfs_fs *sfs, void *buf, size_t len, uint32_t blkno, off_t offset);
int sfs_sync_super(struct sfs_fs *sfs);
int sfs_sync_freemap(struct sfs_fs *sfs);
int sfs_clear_block(struct sfs_fs *sfs, uint32_t blkno, uint32_t nblks);

int sfs_load_inode(struct sfs_fs *sfs, struct inode **node_store, uint32_t ino);

#endif /* !__KERN_FS_SFS_SFS_H__ */

