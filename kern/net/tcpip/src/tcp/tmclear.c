#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tmclear -  返回已经流逝的时间
 *------------------------------------------------------------------------
 */

int
tmclear(int port, void* msg){
	struct tqent   *prev, *ptq;
	int    timespent;
	lock(&tqmutex);
    prev = 0;

    for(ptq = tqhead; ptq != NULL; ptq = ptq->tq_next) {
    	if (ptq->tq_port == port && ptq->tq_msg == msg ) {
    		timespent = /*ctr100*/gettime2() - ptq->tq_time;

    		if (prev) {
    			prev->tq_next = ptq->tq_next;
    		} else {
    			tqhead = ptq->tq_next;
    		}
            //调整tq_next的时间
    		if (ptq->tq_next) {
    			ptq->tq_next->tq_timeleft += ptq->tq_timeleft;
    		}
    		unlock(&tqmutex);
    		freebuf(ptq);
    		return timespent;
    	}
    	prev = ptq;
    }
    unlock(&tqmutex);
    return SYSERR;
}

