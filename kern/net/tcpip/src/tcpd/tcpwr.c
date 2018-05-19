#include <tcpip/h/network.h>

int tcpgetspace(struct tcb *, unsigned int);

/*------------------------------------------------------------------------
 *  tcpwr  -  write urgent and normal data to TCP buffers
 *------------------------------------------------------------------------
 */
int
tcpwr(struct device *pdev, unsigned char *pch, unsigned int len, bool isurg) {
	struct   tcb  *ptcb = (struct tcb *)pdev->dvioblk;
	int      state = ptcb->tcb_state;
	unsigned int sboff;
	int      tocopy;
 
    if (state != TCPS_ESTABLISHED && state != TCPS_CLOSEWAIT) {
    	return SYSERR;
    }

    tocopy = tcpgetspace(ptcb, len);   // acquire tcb_mutex
    if (tocopy <= 0) {
    	return tocopy;
    }
    
    //发送缓冲区中填写数据的位置
    sboff = (ptcb->tcb_sbstart + ptcb->tcb_sbcount) % ptcb->tcb_sbsize;
    if (isurg) {
    	ptcb->tcb_supseq = ptcb->tcb_snext + len -1;
    	ptcb->tcb_flags |= TCBF_SUPOK;
    }

    while(tocopy--) {
    	ptcb->tcb_sndbuf[sboff] = *pch++;
    	++ptcb->tcb_sbcount;
    	if (++sboff >= ptcb->tcb_sbsize) {
    		sboff = 0;//环
    	}
    }
    ptcb->tcb_flags |= TCBF_NEEDOUT;
    tcpwakeup(WRITERS, ptcb);
    unlock(&(ptcb->tcb_mutex));
    

    if (isurg || ptcb->tcb_snext == ptcb->tcb_suna) {
        tcpkick(ptcb);
    }
    return len;
}

