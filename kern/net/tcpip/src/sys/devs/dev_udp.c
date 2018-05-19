#include <defs.h>
#include <stdio.h>
#include <wait.h>
#include <sync.h>
#include <proc.h>
#include <sched.h>
#include <dev.h>
#include <vfs.h>
#include <iobuf.h>
#include <inode.h>
#include <unistd.h>
#include <error.h>
#include <assert.h>

#include <tcpip/h/network.h>

int 
udp_init(struct device* dev) {
    return dginit(dev);
    //return OK;
}

int
udp_open(struct device *dev, uint32_t open_flags, uint32_t arg2) {
    //panic("udp_open not be implemented\n");
	//return -E_UNIMP;
    return OK;
}

int
udp_close(struct device *dev) {
    //panic("udp_close not be implemented\n");
    //return -E_UNIMP;
    return dgclose(dev);
}

int
udp_io(struct device *dev, struct iobuf *iob, bool write) {
    panic("udp_io not be implemented\n");
    return -E_UNIMP;
}

int
udp_ioctl(struct device *dev, int op, void* arg1, void* arg2) {
    //panic("udp_ioctl not be implemented\n");
    //return -E_UNIMP;
    return dgcntl(dev, op, arg1);
}



int udp_write(struct device *dev, char* buff, int len) {
    return dgwrite(dev, (unsigned char*)buff, len);
}

int udp_read(struct device *dev, char* buff, int len) {
    cprintf("udp_read len = %d\n",len);
    return dgread(dev,buff, len);
}


static void
udp_device_init(struct device *dev) {
    dev->d_open = udp_open;
    dev->d_close = udp_close;
    dev->d_io = udp_io;
    dev->d_ioctl = udp_ioctl;
}

void
dev_init_udp() {
    struct inode *node;
    //创建inode，并且设置inode->inode_ops==dev_node_ops
    if ( (node=dev_create_inode())==NULL) {
    	panic("udp: dev_init_tcp failed.\n");
    }

    udp_device_init(vop_info(node, device));
    //devtab[UDP] = vop_info(node, device);

    int ret ;
    if ((ret = vfs_add_dev("udp", node, 0)) != 0) {
    	panic("udp: vfs_add_dev: error %e.\n", ret);
    }
 
}
