#include <tcpip/h/network.h>


/*------------------------------------------------------------------------
 *  tcplq - 设置tcp_listenq
 *------------------------------------------------------------------------
 */
int
tcplq(struct tcb *ptcb, unsigned int lqsize) {
	if (ptcb->tcb_state == TCPS_FREE) {
       return SYSERR;
	}
	ptcb->tcb_lqsize = lqsize;
	if (ptcb->tcb_type == TCPT_SERVER) {
		pdelete(ptcb->tcb_listenq, PTNODISP);
		ptcb->tcb_listenq = pcreate(ptcb->tcb_lqsize);
	}
    return OK;
}

