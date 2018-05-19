#include <tcpip/h/network.h>

#define	MAXECHO	600		/* maximum size of echoed datagram	*/


int32_t 
udpechod(void* args) {
   int dev, len;
   unlock(&Net.sema);
   struct xgram* pxgram1 = kmalloc(sizeof(struct xgram));

   if ((dev=net_dev_open(UDP, ANYFPORT, (void *)UP_ECHO)) == SYSERR) {
   	  cprintf("udpecho: open fails\n");
   	  return SYSERR; 
   }

   while(true) {
      memset(pxgram1->xg_data, 0, U_MAXLEN);
      len = net_dev_read(dev,pxgram1,U_MAXLEN);
      cprintf("udpecho read %d bytes data\n", len);
      cprintf("----------------------------------------------\n");
      cprintf("%s",pxgram1->xg_data);
      cprintf("----------------------------------------------\n");
      net_dev_write(dev, pxgram1, len);
   }
}

