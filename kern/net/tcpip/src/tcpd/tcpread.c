#include <tcpip/h/network.h>

int tcpgetdata(struct tcb *, unsigned char *, unsigned int);

/*------------------------------------------------------------------------
 *  tcpread  -  read one buffer from a TCP pseudo-device
 *------------------------------------------------------------------------
 */
int tcpread(struct device *pdev, char *pch, unsigned int len) {
	struct tcb *ptcb = (struct tcb*)pdev->dvioblk;
	int    state  = ptcb->tcb_state;
	int    cc;

	if (state != TCPS_ESTABLISHED && state != TCPS_CLOSEWAIT) {
		return SYSERR;
	}
retry:
    wait(&(ptcb->tcb_rsema));
    lock(&(ptcb->tcb_mutex));

    if (ptcb->tcb_state == TCPS_FREE) {
    	return SYSERR;
    }
    if (ptcb->tcb_error) {
    	tcpwakeup(READERS, ptcb);
        unlock(&(ptcb->tcb_mutex));
        return ptcb->tcb_error;
    }
   
    if (ptcb->tcb_flags & TCBF_RUPOK) {
    	if (!current->ptcpumode) {
    		current->ptcpumode = true;
    		cc = TCPE_URGENTMODE;
    	} else {
    		cc = tcpgetdata(ptcb, (unsigned char*)pch, len);
    	}
    } else {
    	if (current->ptcpumode) {
    		current->ptcpumode = false;
    		cc = TCPE_NORMALMODE;
    	} else if ( (len > ptcb->tcb_rbcount) &&
    		(ptcb->tcb_flags & TCBF_BUFFER) &&
    		((ptcb->tcb_flags & (TCBF_PUSH|TCBF_RDONE))==0) ) {

    		unlock(&(ptcb->tcb_mutex));
    		goto retry;
    	} else {
    		cc = tcpgetdata(ptcb, (unsigned char*)pch, len);
    	}
    }

    tcpwakeup(READERS, ptcb);
    signal(&(ptcb->tcb_mutex));
    return cc;
}