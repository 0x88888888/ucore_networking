#include <tcpip/h/network.h>

#define ISUOPT(flags)	(!((flags) & ~(TCBF_DELACK|TCBF_BUFFER)))

/*------------------------------------------------------------------------
 *  tcpuopt - set/clear TCP user option flags  
 *------------------------------------------------------------------------
 */
int
tcpuopt(struct tcb *ptcb, unsigned int func, unsigned int flags) {
	if (!ISUOPT(flags)) {
		return SYSERR;
	}
	if (func == TCPC_SOPT){
		//set user-selectable options 
		ptcb->tcb_flags |= flags;
	} else {
		ptcb->tcb_flags &= ~flags;
	}
	return OK;
}
