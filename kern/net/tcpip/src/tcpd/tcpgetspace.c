#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpgetspace  -  wait for space in the send buffer
 *	N.B. - returns with tcb_mutex HELD
 *------------------------------------------------------------------------
 */
int tcpgetspace(struct tcb *ptcb, unsigned int len) {
	if (len > ptcb->tcb_sbsize) {
		return TCPE_TOOBIG; /* we'll never have this much	*/
	}
    
  
	lock(&(ptcb->tcb_mutex));

	while(true) {

		wait(&(ptcb->tcb_ssema));

		if (ptcb->tcb_state == TCPS_FREE) {
			cprintf("tcpgetspace ptcb->tcb_state is FREE\n");
			unlock(&(ptcb->tcb_mutex));
			return SYSERR;  /* gone */
		}
		if (ptcb->tcb_error) {
			cprintf("tcpgetspace, ptcb->tcb_error =%x \n", ptcb->tcb_error);
			tcpwakeup(WRITERS, ptcb);
			unlock(&(ptcb->tcb_mutex));
			return ptcb->tcb_error;
		}
		if (len <= ptcb->tcb_sbsize - ptcb->tcb_sbcount) {	
			unlock(&(ptcb->tcb_mutex));
			return len;
		}
	}
	
}



