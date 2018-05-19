#include <defs.h>
#include <string.h>
#include <stat.h>
#include <dev.h>
#include <inode.h>
#include <unistd.h>
#include <error.h>

/*
 * 本文件的函数都是通过inode->in_ops->* ,转过来的，
 * 然后在各个函数内部，得到相应的device, 
 * 通过inode->in_info.__device_info得到,
 * 然后通过device的各个成员去操作
 */

/*
 * dev_open - Called for each open().
 * inode->in_ops->vop_open( inode, flags)
 */
static int
dev_open(struct inode *node, uint32_t open_flags, uint32_t arg) {
	if (open_flags & (O_CREAT | O_TRUNC | O_EXCL | O_APPEND)) {
        return -E_INVAL;
    }
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);
    /*
     * dev->d_open(dev, open_flags)
     * disk0的 d_open= disk0_open
     * stdin的 d_open= stdin_open
     * stdout的 d_open= stdout_open
     */
    return dop_open(dev, open_flags, arg);
}

/*
 * dev_close - Called on the last close(). Just pass through.
 *
 * 
 * inode->in_ops->vop_close( inode )
 */
static int
dev_close(struct inode *node) {
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);
    /*
     * dev->d_close(dev);
     * disk0的 d_close= disk0_close
     * stdin的 d_close= stdin_close
     * stdout的 d_close = stdout_close
     */
    return dop_close(dev);
}

/*
 * dev_read -Called for read. Hand off to iobuf.
 *
 * inode->in_ops->vop_read(inode, iob)
 */
//static int
int static
dev_read(struct inode *node, struct iobuf *iob) {
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);

    //debug
    /*
    extern int stdout_io(struct device *dev, struct iobuf *iob, bool write);
    extern int disk0_io(struct device *dev, struct iobuf *iob, bool write);
    extern int stdin_io(struct device *dev, struct iobuf *iob, bool write);
    cprintf("in dev_read\n");
    cprintf("disk0_io:%x \n stdin_io:%x stdout_io:%x\n",disk0_io, stdin_io, stdout_io);
    cprintf("dev->d_io:%x\n",dev->d_io);
    */
    /*
     * dev->d_io(dev, iob, 0);
     * disk0的 d_io= disk0_io
     * stdin的 d_io= stdin_io
     * stdout的 d_io= stdout_io
     */
    return dop_io(dev, iob, 0);
}

/*
 * dev_write -Called for write. Hand off to iobuf.
 *
 *
 * inode->in_ops->vop_write(inode, iob)
 */
static int
dev_write(struct inode *node, struct iobuf *iob) {
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);
    /* 
     * dev->d_io(dev, iob, 1);
     *
     * disk0的 d_io= disk0_io
     * stdin的 d_io= stdin_io
     * stdout的 d_io= stdout_io
     */
    return dop_io(dev, iob, 1);
}

/*
 * dev_ioctl - Called for ioctl(). Just pass through.
 *
 * inode->in_ops->vop_ioctl(inode, op, data)
 */
static int
dev_ioctl(struct inode *node, int op, void *data) {
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);
    /*
     * dev->d_ioctl(dev, op, data);
     *
     * disk0的 d_ioctl= disk0_ioctl
     * stdin的 d_ioctl= stdin_ioctl
     * stdout的 d_ioctl= stdout_ioctl
     */
    return dop_ioctl(dev, op, data, NULL);
}

/*
 * dev_fstat - Called for stat().
 *             Set the type and the size (block devices only).
 *             The link count for a device is always 1.
 *
 *
 * inode->in_ops->vop_fstat(inode, stat)
 */
static int
dev_fstat(struct inode *node, struct stat *stat) {
    int ret;
    memset(stat, 0, sizeof(struct stat));
    //node->in_ops->vop_gettype(node, &(stat->st_mode));
    if ((ret = vop_gettype(node, &(stat->st_mode))) != 0) {
        return ret;
    }
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);
    stat->st_nlinks = 1;
    stat->st_blocks = dev->d_blocks;
    stat->st_size = stat->st_blocks * dev->d_blocksize;
    return 0;
}

/*
 * dev_gettype - Return the type. A device is a "block device" if it has a known
 *               length. A device that generates data in a stream is a "character
 *               device".
 *
 *
 * inode->in_ops->vop_gettype(inode, type_store)
 */
static int
dev_gettype(struct inode *node, uint32_t *type_store) {
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);
    *type_store = (dev->d_blocks > 0) ? S_IFBLK : S_IFCHR;
    return 0;
}

/*
 * dev_tryseek - Attempt a seek.
 *               For block devices, require block alignment.
 *               For character devices, prohibit seeking entirely.
 *
 *
 * inode->in_ops->vop_tryseek(inode , pos)
 */
static int
dev_tryseek(struct inode *node, off_t pos) {
    //dev=node->__device_info;
    struct device *dev = vop_info(node, device);
    if (dev->d_blocks > 0) {
        if ((pos % dev->d_blocksize) == 0) {
            if (pos >= 0 && pos < dev->d_blocks * dev->d_blocksize) {
                return 0;
            }
        }
    }
    return -E_INVAL;
}

/*
 * dev_lookup - Name lookup.
 *
 * One interesting feature of device:name pathname syntax is that you
 * can implement pathnames on arbitrary devices. For instance, if you
 * had a graphics device that supported multiple resolutions (which we
 * don't), you might arrange things so that you could open it with
 * pathnames like "video:800x600/24bpp" in order to select the operating
 * mode.
 *
 * However, we have no support for this in the base system.
 *
 *
 * inode->in_ops->vop_lookup(node,path, node_store)
 */
static int
dev_lookup(struct inode *node, char *path, struct inode **node_store) {
    if (*path != '\0') {
        return -E_NOENT;
    }
    vop_ref_inc(node);
    *node_store = node;
    return 0;
}

/*
 * Function table for device inodes.
 */
static const struct inode_ops dev_node_ops = {
    .vop_magic                      = VOP_MAGIC,
    .vop_open                       = dev_open,
    .vop_close                      = dev_close,
    .vop_read                       = dev_read,
    .vop_write                      = dev_write,
    .vop_fstat                      = dev_fstat,
    .vop_ioctl                      = dev_ioctl,
    .vop_gettype                    = dev_gettype,
    .vop_tryseek                    = dev_tryseek,
    .vop_lookup                     = dev_lookup,
    .vop_lookup_parent              = NULL_VOP_NOTDIR,
};

/*
 dev_init_stdin();
 dev_init_stdout();
 dev_init_disk0();
 dev_init_e1000();
*/
#define init_device(x)                                  \
    do {                                                \
        extern void dev_init_##x(void);                 \
        dev_init_##x();                                 \
    } while (0)

//struct device* devtab[2] ;


/* dev_init - Initialization functions for builtin vfs-level devices. */
/*
 * 设置stdin、stdout、disk0
 *
 */
void
dev_init(void) {
    
   // init_device(null);
    init_device(stdin);  // dev_init_stdin
    init_device(stdout); // dev_init_stdout
    init_device(disk0);  // dev_init_disk0
    /*
    init_device(e1000);  // dev_init_e1000
    init_device(tcp);    // dev_init_tcp
    init_device(udp);    // dev_init_udp
    */
}
/* dev_create_inode - Create inode for a vfs-level device. */
struct inode *
dev_create_inode(void) {
    struct inode *node;
    //   node = __alloc_inode(inode_type_device_info) ;
    if ((node = alloc_inode(device)) != NULL) {
        //inode_init(node, &dev_node_ops, NULL);
        //node->inode_ops==dev_node_ops
        vop_init(node, &dev_node_ops, NULL);
    }
    vop_info(node,device)->dvnum = 0;
    bzero(vop_info(node, device)->name,10);
    return node;
}

