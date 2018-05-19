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
tcp_init(struct device *dev) {
   return tcpinit(dev);
   //return OK;
}

int
tcp_open(struct device *dev, uint32_t open_flags, uint32_t arg2) {
	//panic("tcp_open not be implemented\n");
	//return -E_UNIMP;
    return OK;
}

int
tcp_close(struct device *dev) {
    return tcpclose(dev);
    //panic("tcp_close not be implemented\n");
    //return -E_UNIMP;
}

int
tcp_io(struct device *dev, struct iobuf *iob, bool write) {
    panic("tcp_io not be implemented\n");
	return -E_UNIMP;
}

int
tcp_ioctl(struct device *dev, int op, void* arg1, void* arg2) {
    //panic("tcp_ioctl not be implemented\n");
    return tcpcntl(dev, op, arg1, arg2);
    
}


int tcp_write(struct device *dev, char* buff, int len) {
    return tcpwrite(dev, (unsigned char*)buff, len);
}

int tcp_read(struct device *dev, char* buff, int len) {
    return tcpread(dev, buff, len);
}


static void
tcp_device_init(struct device *dev) {
    dev->d_open = tcp_open;
    dev->d_close = tcp_close;
    dev->d_io = tcp_io;
    dev->d_ioctl = tcp_ioctl;
}

void
dev_init_tcp() {
    struct inode *node;
    //创建inode，并且设置inode->inode_ops==dev_node_ops
    if ( (node=dev_create_inode())==NULL) {
    	panic("tcp: dev_init_tcp failed.\n");
    }

    tcp_device_init(vop_info(node, device));
    //devtab[TCP] = vop_info(node, device);

    int ret ;
    if ((ret = vfs_add_dev("tcp", node, 0)) != 0) {
    	panic("tcp: vfs_add_dev: error %e.\n", ret);
    }
 
}
