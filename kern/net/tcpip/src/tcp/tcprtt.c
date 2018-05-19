#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcprtt - do round trip time estimates & cong. window processing
 *
 *  根据估算出来的时间，得到ptcb->tcb_cwnd
 *------------------------------------------------------------------------
 * tcpacked
 *     tcprtt
 *
 */
int
tcprtt(struct tcb *ptcb) {
	int rrt;     /* raw round trip */
	int delta;   /* deviation from smoothed */
    
    //rrt为已经流逝的时间
	rrt = tmclear(tcps_oport, MKEVENT(RETRANSMIT, ptcb-&tcbtab[0]));

	if (rrt != SYSERR && ptcb->tcb_ostate != TCPO_REXMT) {
		if (ptcb->tcb_srt == 0) {
			ptcb->tcb_srt = rrt<<3;  /* prime the pump */
		}
		/*
		 * "srt" is scaled X 8 here, so this is really:
		 * delta = rrt - srt
		 */
		delta = rrt - (ptcb->tcb_srt>>3);
		ptcb->tcb_srt += delta;   /* srt = srt + delta/8 */
		if (delta < 0) {
			delta = -delta;
		}
		/*
		 * "rtde" is scaled X 4 here, so this is really:
		 *  rtde = rtde + (|delta| - rtde)/4
		 */
		ptcb->tcb_rtde += delta - (ptcb->tcb_rtde>>2);
		/*
		 * "srt" is scaled X 8, rtde scaled X 4, so this is:
		 * rto = srt + 2 * rtde
		 */
		ptcb->tcb_rexmt = ((ptcb->tcb_srt>>2)+ptcb->tcb_rtde)>>1;
		if (ptcb->tcb_rexmt < TCP_MINRXT) {
			ptcb->tcb_rexmt = TCP_MINRXT;
		}
	}

	if (ptcb->tcb_cwnd < ptcb->tcb_ssthresh) {
		//一个一个的加
		ptcb->tcb_cwnd += ptcb->tcb_smss;
	} else {
        // mss * mss / cwnd
		ptcb->tcb_cwnd += (ptcb->tcb_smss * ptcb->tcb_smss) / ptcb->tcb_cwnd;
	}
    return OK;
}

   