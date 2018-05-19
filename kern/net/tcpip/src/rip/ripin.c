#include <tcpip/h/network.h>
#include <sysfile.h>
int	rippid = BADPID;
bool dorip = false;
mutex riplock;


/*------------------------------------------------------------------------
 *  rip  -  do the RIP route exchange protocol
 *------------------------------------------------------------------------
 */
int32_t 
rip(void* args){
   struct xgram ripbuf;
   struct rip   *prip;
   int    fd, len, pid;
   unlock(&Net.sema);
   //devtab中的索引                   
   fd = net_dev_open(UDP, ANYFPORT, (void*)UP_RIP);
   if (fd == -E_INVAL) {
   	  panic("rip: cannot open rip port\n");
   }
   mutex_init(&riplock);
   if (gateway) {
   	   pid=kernel_thread(ripout, NULL, 0);
   	   set_pid_name(pid, "ripout");
   }


   while(true) {
   	   len = net_dev_read(fd, &ripbuf, sizeof(ripbuf));
   	   if (len < 0) {
             cprintf("rip: net_dev_read error %d\n",len);
   	   	 continue;
   	   }
   	   prip = (struct rip*) ripbuf.xg_data;
   	   if (ripcheck(prip, len) != OK){
   	   	  continue;
   	   }
   	   switch (prip->rip_cmd) {
   	   	    case RIP_RESPONSE:
   	   	        if (ripbuf.xg_fport == UP_RIP) {
   	   	        	 riprecv(prip, len, ripbuf.xg_fip);
   	   	        }
   	   	        break;
   	   	    case RIP_REQUEST:
   	   	        if (gateway || ripbuf.xg_fport != UP_RIP) {
   	   	        	 riprepl(prip, len, ripbuf.xg_fip, ripbuf.xg_fport);
   	   	        }
   	   	        break;
   	   	    default:
   	   	        break;
   	   } 

   }
}

