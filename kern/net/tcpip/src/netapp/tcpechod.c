#include <tcpip/h/network.h>

int32_t echoprocess(void* clientfd);

int32_t 
tcpechod(void* args) { 
  unlock(&Net.sema);

  int dev;
 
  dev = net_dev_open(TCP, ANYFPORT, (void *)7);


  net_dev_control(dev, TCPC_LISTENQ, 10);


  while(true) {
    
  	int clientfd;
    
    clientfd = net_dev_control(dev, TCPC_ACCEPT, 1);

    if (clientfd == SYSERR) {
		   break;
    }

	  //启动进程，处理连接
	  kernel_thread(echoprocess, (void*)clientfd, 0);
  }

  return 0;
}


int32_t echoprocess(void* clientfdarg) {
     int len, wr;
     char  buf[512];
     int clientfd = (int)clientfdarg;

     while(true) {
         len = net_dev_read(clientfd, buf, sizeof(buf));

         cprintf("echod process read, len=%x\n", len);
         if (len > 0) {
          buf[len] = '\0';
          cprintf("echod process read buf= %s\n", buf);
         }

         if (len == TCPE_URGENTMODE || len == TCPE_NORMALMODE) {
         	continue;
         }
         if (len <= 0 || len == SYSERR) {
         	  break;
         }
         wr = net_dev_write(clientfd, buf, len);
         if (wr <= 0 || wr == SYSERR) {
         	break;
         }
     }
     net_dev_close(clientfd);
     return OK;
}

