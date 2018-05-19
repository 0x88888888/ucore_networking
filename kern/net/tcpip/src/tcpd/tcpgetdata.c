#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpgetdata  -  copy data from a TCP receive buffer to a user buffer
 *------------------------------------------------------------------------
 */
int
tcpgetdata(struct tcb *ptcb, unsigned char *pch, unsigned int len) {
	//tcpseq        seq;
	unsigned int   cc;

	//seq = ptcb->tcb_rnext - ptcb->tcb_rbcount;  
	/* start sequence */
	if (ptcb->tcb_flags & TCBF_RUPOK) {
		int nbc, ubc; /* normal & urgent byte counts	*/
		nbc = ptcb->tcb_rnext - ptcb->tcb_rupseq - 1;
		if (nbc >= 0) { /* urgent boundary in buffer */
		    ubc = ptcb->tcb_rbcount - nbc;
		    if ((int)len >= ubc) {
		    	len = ubc;
		    	ptcb->tcb_flags &= ~TCBF_RUPOK;
		    }
		}
	}
    
    //复制数据到用户缓冲区
	for(cc=0; ptcb->tcb_rbcount && cc < len;) {
       *pch++ = ptcb->tcb_rcvbuf[ptcb->tcb_rbstart];
       --ptcb->tcb_rbcount;
       if (++ptcb->tcb_rbstart >= ptcb->tcb_rbsize){
       	  ptcb->tcb_rbstart = 0;
       }
       ++cc;
	}

	if (ptcb->tcb_rbcount == 0) {
		//缓冲区没有数据，不提醒用户层
		ptcb->tcb_flags &= ~TCBF_PUSH;
	}

	/*
	 * open the receive window, if it's closed and we've made
	 * enough space to fit a segment.
	 */
	if (SEQCMP(ptcb->tcb_cwin, ptcb->tcb_rnext) <=0 &&
		tcprwindow(ptcb)) {
		ptcb->tcb_flags |= TCBF_NEEDOUT;
	    tcpkick(ptcb);
	}
    return cc;
}