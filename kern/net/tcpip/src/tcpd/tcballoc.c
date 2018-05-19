#include <tcpip/h/network.h>


/*------------------------------------------------------------------------
 * tcballoc - allocate a Transmission Control Block
 *------------------------------------------------------------------------
 */
struct tcb *tcballoc(void) {
	struct tcb *ptcb;
	int slot;

	lock(&tcps_tmutex);
    /* look for a free TCB */

    for (ptcb=&tcbtab[0], slot = 0; slot < Ntcp; ++slot, ++ptcb) {
    	if (ptcb->tcb_state == TCPS_FREE)
    		break;
    }
    if (slot < Ntcp) {
    	//tcp连接的初始状态 CLOSED
    	ptcb->tcb_state = TCPS_CLOSED;
    	mutex_init(& (ptcb->tcb_mutex));
    } else {
    	ptcb = (struct tcb *)SYSERR;
    }
    unlock(&tcps_tmutex);
    return ptcb;
}

