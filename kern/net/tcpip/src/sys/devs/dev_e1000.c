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

#include <e1000.h>
#include <tcpip/h/network.h>
// Neth == 1
struct etblk eth[Neth];   /* device control blocks */

#define E1000_BLKSIZE     PGSIZE
#define E1000_BUFSIZE     (E1000_BLKSIZE * 1 )

/*
static char* e1000_in_buff;
static char* e1000_out_buff;
*/
static semaphore_t e1000_sem;

/*
static void
lock_e1000(void) {
	down(&(e1000_sem));
}

static void
unlock_e1000(void) {
	up(&(e1000_sem));
}
*/

int 
dev_e1000_init(struct device *devptr) {
  struct etblk* etptr = &eth[devptr->dvminor];//eth[0];
  devptr->dvioblk = (char*)etptr;
  cprintf("dev_e1000_init dvminor=%d etptr=%x\n",devptr->dvminor ,etptr);
  memset(etptr, 0 ,sizeof(struct etblk));
  etptr->etdnum = devptr->dvnum;
  e1000_mac(etptr->etpaddr);
  const char* sr= EP_BRC;
  blkcopy(etptr->etbcast, EP_BRC, EP_ALEN);
  cprintf("EP_BRC= %02x:%02x:%02x:%02x:%02x:%02x dvminor=%d dvnum=%d\n",
    sr[0],
    sr[1],
    sr[2],
    sr[3],
    sr[4],
    sr[5],
    devptr->dvminor,
    devptr->dvnum);

  cprintf("eth[%d].baddr: %02x:%02x:%02x:%02x:%02x:%02x \n",
    devptr->dvminor,
    etptr->etbcast[0],
    etptr->etbcast[1],
    etptr->etbcast[2],
    etptr->etbcast[3],
    etptr->etbcast[4],
    etptr->etbcast[5]);

  etptr->etdescr = "PKTDRV";       
  etptr->etdev = devptr;           

  mutex_init(&(etptr->etrsem));
  mutex_init(&(etptr->etwsem));
  
  //

  return OK;
}


int
dev_e1000_open(struct device *dev, uint32_t open_flags, uint32_t arg2) {
	return dev->dvnum;;
}

int
dev_e1000_close(struct device *dev) {
    return OK;
}

int
dev_e1000_ioctl(struct device *dev, int op, void* arg1, void* arg2) {
    return -E_UNIMP;
}


int 
dev_e1000_write(struct device *dev, char* buff, int len) {
    struct ep* pep=(struct ep*)buff;
    ep_type(pep) = htons(ep_type(pep));
    //struct etblk* petblk =(struct etblk*) dev->dvioblk;
    //blkcopy(ep_src(pep), petblk->etpaddr, EP_ALEN);

    return e1000_transmit((uint8_t*)buff, len);
}

int
dev_e1000_read(struct device *dev, char* buff, int len) {
    int result= e1000_recv((uint8_t*)buff, len);
    return result;
}


int
e1000_read(struct iobuf *iob) { 
  int result= e1000_recv(iob->io_base, iob->io_len);
  return result;
}

int
e1000_write(struct iobuf *iob) {
  int result= e1000_transmit(iob->io_base, iob->io_len);
  return result;
}

int
dev_e1000_io(struct device *dev, struct iobuf *iob, bool write) {
   int result;
   cprintf("dev_e1000_io \n");
   //lock_e1000();
   if (write) {
       result=e1000_write(iob);
   } else {
       result=e1000_read(iob);
   }
   //panic("e1000_io not be implemented\n");
   //unlock_e1000();
   return result;
}


static void
e1000_device_init(struct device *dev) {
  //struct etblk  *etptr;

	sem_init(&e1000_sem, 1);
  /*
  devtab[EC0] = dev;
  dev->dvminor = 0; // 表eth中的索引
  dev->dvnum = 1;   // 表debtab中的索引

  etptr = &eth[dev->dvminor];      
  
  dev->dvioblk = etptr;    // ethblk
  cprintf("dev->dvioblk=%x\n",etptr);
  //set dev's etblk
  memset(etptr, 0 ,sizeof(struct etblk));
  etptr->etdnum = dev->dvnum;
  blkcopy(etptr->etbcast, EP_BRC, EP_ALEN);
  etptr->etdescr = "PKTDRV";       
  etptr->etdev = dev;           

  mutex_init(&(etptr->etrsem));
  mutex_init(&(etptr->etwsem));

//
    dev->d_blocks = 0;
    dev->d_blocksize = 1;
    dev->d_open = dev_e1000_open;
    dev->d_close =dev_e1000_close;
    dev->d_io = dev_e1000_io;
    dev->d_ioctl = dev_e1000_ioctl;
  
  */
 // cprintf("sizeof Eaddr %d etptr=%x  etptr->etpaddr=%x\n",sizeof(Eaddr), etptr,etptr->etpaddr);
  
}


/*
static const struct inode_ops e1000_node_ops = {
    .vop_magic                      = VOP_MAGIC,
    .vop_open                       = vop_e1000_open,
    .vop_close                      = vop_e1000_close,
    .vop_read                       = vop_e1000_read,
    .vop_write                      = vop_e1000_write,
};
*/

void
dev_init_e1000(void) {
	
  struct inode *node;
  //   node = __alloc_inode(inode_type_device_info) ;
  if ((node = dev_create_inode()) == NULL) {
       panic("stdout: dev_init_e1000 failed.\n");
  }

  e1000_device_init(vop_info(node, device));

  //devtab[EC0 ]=vop_info(node, device);

  int ret;
  if ((ret = vfs_add_dev("e1000", node, 0)) != 0) {
      panic("e1000: vfs_add_dev: error %e.\n", ret);
  }
  
}






