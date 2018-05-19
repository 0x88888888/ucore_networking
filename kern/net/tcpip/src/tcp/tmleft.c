#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tmleft -  how much time left for this timer?
 *------------------------------------------------------------------------
 *
 * 得到到事件发生时剩余的时间
 */
int
tmleft(int port, void *msg) {
	struct tqent  *tq;
	int    timeleft = 0;

    if (tqhead == NULL) {
    	return 0;
    }
    lock(&tqmutex);
	for (tq = tqhead; tq != NULL; tq = tq->tq_next) {
		timeleft += tq->tq_timeleft;
		if (tq->tq_port == port && tq->tq_msg == msg) {
			unlock(&tqmutex);
			return timeleft;
		}
	}
	unlock(&tqmutex);
	return 0;
}

