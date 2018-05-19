#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpserver - do a TCP passive open
 *------------------------------------------------------------------------
 */
int
tcpserver(struct tcb *ptcb, unsigned short lport) {
	if (lport == ANYLPORT ) {
		ptcb->tcb_state = TCPS_FREE;
		unlock(&(ptcb->tcb_mutex));
		return SYSERR;
	}
	ptcb->tcb_type = TCPT_SERVER;
	ptcb->tcb_lport = lport;
	ptcb->tcb_state = TCPS_LISTEN;
	ptcb->tcb_lqsize = tcps_lqsize;
	ptcb->tcb_listenq = pcreate(ptcb->tcb_lqsize);
	ptcb->tcb_smss = 0;
	unlock(&ptcb->tcb_mutex);
	return ptcb->tcb_dvnum;
}

