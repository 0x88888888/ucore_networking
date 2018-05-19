#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tmset -  set a fast timer
 *------------------------------------------------------------------------
 */
int
tmset(int port, int portlen, void *msg, int time) {
	struct tqent  *ptq, *newtq, *tq;

	newtq =(struct tqent *)kmalloc(sizeof(struct tqent));
	newtq->tq_timeleft = time;
	newtq->tq_time = gettime2();//ctr100;
	newtq->tq_port = port;
	newtq->tq_portlen = portlen;
	newtq->tq_msg = msg;
	newtq->tq_next = NULL;
    
    /* clear duplicates */
    tmclear(port, msg);

    lock(&(tqmutex));
    if (tqhead == NULL) {
    	tqhead =newtq;
        //wakeup_pid(tqpid);
    	//resume(tqpid);
        unlock(&tqmutex);
        return OK;
    }

    /* search the list for our spot */
    //按照时间次序，插入tqhead链表
    for (ptq=0, tq=tqhead; tq;tq=tq->tq_next) {
    	if (newtq->tq_timeleft < tq->tq_timeleft){
            //找到
    		break;
    	}
        //设置tq_timeleft
    	newtq->tq_timeleft -= tq->tq_timeleft;
    	ptq = tq;
    }
    //链接好
    newtq->tq_next = tq;
    if(ptq){
    	ptq->tq_next = newtq;
    } else {
    	tqhead = newtq;
    }
    if (tq) {
    	tq->tq_timeleft -= newtq->tq_timeleft;
    }
    unlock(&(tqmutex));
    return OK;
}

