#include <defs.h>
//#include <kmalloc.h>
#include <slab.h>
#include <sem.h>
#include <vfs.h>
#include <dev.h>
#include <file.h>
#include <sfs.h>
#include <inode.h>
#include <assert.h>
//called when init_main proc start
void
fs_init(void) {
    /*
     * 设置互斥量 bootfs_sem,vdev_list_sem
     * 初始化设备链表vdev_list
     */
	vfs_init();
    /*
     * 初始化 disk0,stdin,stdout三个设备
     * 1.创建代表设备的inode
     *      inode->in_ops=dev_node_ops
     *      inode->fs=NULL
     *      inode->ref_count = 1;
     *      inode->ref_open = 0;
     * 2.分别设置stdin,stdout,disk0 device对象的成员变量 
     * 3.将设备添加到全局列表vdev_list中去，名称分别为 stdin,stdout,disk0
     */
	dev_init();
    /*
     * disk0设备为 sfs
     * 1.设置name="disk0"的vfs_dev_t 的 fs.fs_type=fs_type_sfs_info 
     * 2.从磁盘disk0的SFS_BLKN_SUPER和SFS_BLKN_FREEMAP初始化相应的信息
     *   
     */
	sfs_init();
}

void
fs_cleanup(void) {
    vfs_cleanup();
}

void
lock_files(struct files_struct *filesp) {
    down(&(filesp->files_sem));
}

void
unlock_files(struct files_struct *filesp) {
    up(&(filesp->files_sem));
}
//Called when a new proc init
/*
 * fs_copy
 * proc_init
 *      files_create
 *
 */
struct files_struct *
files_create(void) {
	//cprintf("[files_create]\n");
	static_assert((int)FILES_STRUCT_NENTRY > 128);
	struct files_struct *filesp;
	if ((filesp = kmalloc(sizeof(struct files_struct) + FILES_STRUCT_BUFSIZE)) != NULL) {
		filesp->pwd = NULL;
		filesp->fd_array = (void *)(filesp + 1);
		filesp->files_count = 0;
		sem_init(&(filesp->files_sem), 1);
		fd_array_init(filesp->fd_array);
	}
	return filesp;
}
//Called when a proc exit
void
files_destroy(struct files_struct *filesp) {
//    cprintf("[files_destroy]\n");
    assert(filesp != NULL && files_count(filesp) == 0);
    if (filesp->pwd != NULL) {
        vop_ref_dec(filesp->pwd);
    }
    int i;
    struct file *file = filesp->fd_array;
    for (i = 0; i < FILES_STRUCT_NENTRY; i ++, file ++) {
        if (file->status == FD_OPENED) {
            fd_array_close(file);
        }
        assert(file->status == FD_NONE);
    }
    kfree(filesp);
}

void
files_closeall(struct files_struct *filesp) {
//    cprintf("[files_closeall]\n");
    assert(filesp != NULL && files_count(filesp) > 0);
    int i;
    struct file *file = filesp->fd_array;
    //skip the stdin & stdout
    for (i = 2, file += 2; i < FILES_STRUCT_NENTRY; i ++, file ++) {
        if (file->status == FD_OPENED) {
            fd_array_close(file);
        }
    }
}

int
dup_fs(struct files_struct *to, struct files_struct *from) {
//    cprintf("[dup_fs]\n");
    assert(to != NULL && from != NULL);
    assert(files_count(to) == 0 && files_count(from) > 0);
    if ((to->pwd = from->pwd) != NULL) {
        vop_ref_inc(to->pwd);
    }
    int i;
    struct file *to_file = to->fd_array, *from_file = from->fd_array;
    for (i = 0; i < FILES_STRUCT_NENTRY; i ++, to_file ++, from_file ++) {
        if (from_file->status == FD_OPENED) {
            /* alloc_fd first */
            to_file->status = FD_INIT;
            fd_array_dup(to_file, from_file);
        }
    }
    return 0;
}

