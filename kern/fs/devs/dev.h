#ifndef __KERN_FS_DEVS_DEV_H__
#define __KERN_FS_DEVS_DEV_H__

#include <defs.h>

struct inode;
struct iobuf;

/*
 * Filesystem-namespace-accessible device.
 * d_io is for both reads and writes; the iobuf will indicates the direction.
 */
struct device {
	int	dvnum; // debtab中的索引
	char name[10];

    size_t d_blocks;
    size_t d_blocksize;
    int (*d_init) (struct device *dev);
    int (*d_open)(struct device *dev, uint32_t open_flags, uint32_t arg2);
    int (*d_close)(struct device *dev);
    int (*d_io)(struct device *dev, struct iobuf *iob, bool write);
    int (*d_write)(struct device *dev, char* buff, int len);
    int (*d_read)(struct device *dev, char* buff, int len);
    int (*d_ioctl)(struct device *dev, int op, void* arg1, void* arg2); 
    
    //tcpinit,dginit中设置
    char* dvioblk;  // ethblk,tcb , dgblk
    //在配置文件中设定
    int	 dvminor; // eth中的索引,tcb中的索引,upqs中的索引
    
};

#define dop_open(dev, open_flags,arg2)           ((dev)->d_open(dev, open_flags,arg2))
#define dop_close(dev)                      ((dev)->d_close(dev))
#define dop_io(dev, iob, write)             ((dev)->d_io(dev, iob, write))
#define dop_ioctl(dev, op, arg1, arg2)            ((dev)->d_ioctl(dev, op, arg1, arg2))

void dev_init(void);
struct inode *dev_create_inode(void);

//extern struct device* devtab[2] ;


#endif /* !__KERN_FS_DEVS_DEV_H__ */

