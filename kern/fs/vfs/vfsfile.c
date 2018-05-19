#include <defs.h>
#include <string.h>
#include <vfs.h>
#include <inode.h>
#include <unistd.h>
#include <error.h>
#include <assert.h>


// open file in vfs, get/create inode for file with filename path.
/*
 *
 * sys_open->
 *   sysfile_open
 *      file_open
 *          vfs_open
 */
int
vfs_open(char *path, uint32_t open_flags, uint32_t arg2, struct inode **node_store) {
	bool can_write = 0;
	switch (open_flags & O_ACCMODE) {
		case O_RDONLY:
		    break;
		case O_WRONLY:
		    break;
		case O_RDWR:
		    can_write = 1;
		    break;
		default:
		    return -E_INVAL;    
	}

    if (open_flags & O_TRUNC) {
        if (!can_write) {
            return -E_INVAL;
        }
    }
    int ret;
    struct inode *node;
    bool excl = (open_flags & O_EXCL) != 0;
    bool create = (open_flags & O_CREAT) != 0;
    //根据路径查找相应的inode
    ret = vfs_lookup(path, &node);

    if (ret != 0) {
        // ret == -16 表示 no such file or directory
    	if (ret == -16 && (create)) {
    		char *name;
    		struct inode *dir;
    		if ((ret = vfs_lookup_parent(path, &dir, &name)) != 0) {
                //cprintf("ret =%d path:%s\n",ret, path);
                //lookup parent路径失败
    			return ret;
    		}
			//在parent dir 下创建 文件
            //目前dev_node_ops,sfs_node_dirops,sfs_node_fileops都没有设置相应的create函数
            //dir->in_ops->vop_create(dir,name,excl,&node)
    		ret = vop_create(dir, name, excl, &node);
    	} else return ret;
    } else if (excl && create) {
    	return -E_EXISTS;
    }
    assert(node != NULL);
    
    /*
     * node->in_ops->vop_open(node, open_flags)
     * sfs_openfile(node, open_flags)
     */
    if ((ret = vop_open(node, open_flags, arg2)) != 0) {
    	vop_ref_dec(node);
    	return ret;
    }

    vop_open_inc(node);
    if (open_flags & O_TRUNC || create) {
        /*
         * node->in_ops->vop_truncate==sfs_truncfile;
         * sfs_truncfile(node, 0)
         */
    	if ((ret = vop_truncate(node, 0)) != 0) {
            vop_open_dec(node);
            vop_ref_dec(node);
            return ret;
        }
    }
    *node_store = node;
    return 0;
}

// close file in vfs
int
vfs_close(struct inode *node) {
    vop_open_dec(node);
    vop_ref_dec(node);
    return 0;
}

// unimplement
int
vfs_unlink(char *path) {
    return -E_UNIMP;
}

// unimplement
int
vfs_rename(char *old_path, char *new_path) {
    return -E_UNIMP;
}

// unimplement
int
vfs_link(char *old_path, char *new_path) {
    return -E_UNIMP;
}

// unimplement
int
vfs_symlink(char *old_path, char *new_path) {
    return -E_UNIMP;
}

// unimplement
int
vfs_readlink(char *path, struct iobuf *iob) {
    return -E_UNIMP;
}

// unimplement
int
vfs_mkdir(char *path){
    int ret;
    char *name;
    struct inode *dir;
    if ((ret = vfs_lookup_parent(path, &dir, &name)) != 0) {
        return ret;
    }
    ret = vop_mkdir(dir, name);
    vop_ref_dec(dir);
    return ret;
    //return -E_UNIMP;
}
