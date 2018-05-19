#include <tcpip/h/network.h>

#define	STGRAN	1		/* Timer granularity (delay) in seconds	*/

/*
 *处理各个表中的数据 arptable, ipfq->ipf_q(ip分片数据),rttable(rt_metric过期数据)
 */
int32_t slowtimer_proc(void* args) {
   long lasttime, now; // previous and current times in seconds
   int  delay;
   unlock(&Net.sema);
   lasttime = gettime2();
   while(true ) {
   	  do_sleep(STGRAN);
   	  now = gettime2();
   	  delay = now - lasttime;
   	  lasttime = now;

   	  if (delay == 0) {
   	  	delay = 1;
   	  }
   	  arptimer(delay);
   	  ipftimer(delay);
        rttimer(delay);
   }
   return OK;
}


