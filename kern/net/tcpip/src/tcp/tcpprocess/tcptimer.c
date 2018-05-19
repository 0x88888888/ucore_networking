#include <tcpip/h/network.h>


mutex  tqmutex;
int    tqpid;
struct tqent  *tqhead;

/*------------------------------------------------------------------------
 *  tcptimer -  TCP timer process
 *------------------------------------------------------------------------
 */
int32_t tcptimer(void* args) {
	long now, lastrun;     /* times from system clock	*/
	int delta;             /* time since last iteration	*/
	struct tqent   *tq;    /* temporary delta list ptr	*/
    
    lastrun = gettime2(); //ctr100;      /* initialize to "now"       */
    mutex_init(&tqmutex);  /* mutual exclusion semaphore */
    tqpid = getpid();   
    unlock(&Net.sema);      /* start other network processes */

    while(true) {
    	//do_sleep(TIMERGRAN);
        /*
    	if(tqhead == 0) {
    		suspend(tqpid);
    	}
        */
        
        while(tqhead == NULL) {
            schedule();
            lastrun = 0;
        }
        

        lock(&tqmutex);
        now = gettime2();
        if (lastrun !=0) {
            delta = now - lastrun; 
        } else {
            delta = 0;
        }
        

        lastrun = now;
        
        /*
        while (tqhead != NULL && tqhead->tq_timeleft <= delta) {
            delta -= tqhead->tq_timeleft;
            if (pcount(tqhead->tq_port) <= tqhead->tq_portlen) {
                psend(tqhead->tq_port, (int)tqhead->tq_msg);
            }
            tq = tqhead;
            tqhead = tqhead->tq_next;
            freebuf(tq);
        }
        */
        if (tqhead) {
            tqhead->tq_timeleft -= delta;
            if (tqhead->tq_timeleft <=0 && pcount(tqhead->tq_port) <= tqhead->tq_portlen) {
                psend(tqhead->tq_port, (int)tqhead->tq_msg);
            }
            tq = tqhead;
            tqhead = tq->tq_next;
            freebuf(tq);
        }

        unlock(&tqmutex);
    }
    
}



