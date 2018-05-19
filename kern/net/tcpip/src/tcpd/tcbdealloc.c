#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcbdealloc - deallocate a TCB and free its resources
 *	ASSUMES ptcb->tcb_mutex HELD
 *------------------------------------------------------------------------
 */
int
tcbdealloc(struct tcb *ptcb) {
	if (ptcb->tcb_state == TCPS_FREE) {
		return OK;
	}
	switch(ptcb->tcb_type) {

		case TCPT_CONNECTION://
		    tcpkilltimers(ptcb);
		    wakeup_all(&(ptcb->tcb_ocsem));
		    wakeup_all(&(ptcb->tcb_ssema));
		    wakeup_all(&(ptcb->tcb_rsema));
		    
            freebuf((ptcb->tcb_rcvbuf));
		    freebuf((ptcb->tcb_sndbuf));
		    ptcb->tcb_sbsize = 0;
		    
		    ptcb->tcb_rbsize = 0;
		    if ( (ptcb->tcb_rsegq != NULL) && (!is_emptyq(ptcb->tcb_rsegq)) ) {
		    	freeq(ptcb->tcb_rsegq);
		    }
		    break;
		case TCPT_SERVER:
		    pdelete(ptcb->tcb_listenq, 0);
		    break;
		default:
		    unlock(&ptcb->tcb_mutex);
		    return SYSERR;
	}
	wakeup_all(&ptcb->tcb_mutex);
	ptcb->tcb_state = TCPS_FREE;
	
	return OK;
}



