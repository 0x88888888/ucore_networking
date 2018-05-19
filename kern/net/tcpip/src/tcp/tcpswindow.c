#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpswindow -  handle send window updates from remote
 *                根据pep的数据，来调整发送窗口 tcb_swindow
 *------------------------------------------------------------------------
 *
 *  
 *   tcpclosewait
 *   tcpestablished
 *   tcpfin1
 *       tcpswindow
 */
int
tcpswindow(struct tcb *ptcb, struct ep *pep) {
	struct ip    *pip  = (struct ip *) pep->ep_data;
	struct tcp   *ptcp = (struct tcp *)pip->ip_data;
    tcpseq       wlast, owlast;

    if (SEQCMP(ptcp->tcp_seq, ptcb->tcb_lwseq) < 0){
        //未按序到达
    	return OK;
    }
    if (SEQCMP(ptcp->tcp_seq, ptcb->tcb_lwseq) == 0 &&
    	SEQCMP(ptcp->tcp_ack, ptcb->tcb_lwack) < 0) {
        //未按序到达
    	return OK;
    }
    /* else, we have a send window update */
    
    /* compute the last sequence of the new and old windows */
    
    //旧的对方可接受的数据最大sequence
    owlast = ptcb->tcb_lwack + ptcb->tcb_swindow;
    
    //新的对方可接受的数据最大sequence
    wlast = ptcp->tcp_ack + ptcp->tcp_window;

    ptcb->tcb_swindow = ptcp->tcp_window;
    ptcb->tcb_lwseq = ptcp->tcp_seq; // 
    ptcb->tcb_lwack = ptcp->tcp_ack; // 
    if (SEQCMP(wlast, owlast) <= 0) {
    	return OK;
    }

    if(ptcb->tcb_ostate == TCPO_PERSIST) {
       //因为有窗口空间了,所以将PERSIST状态转回 XMIT状态
       tmclear(tcps_oport, MKEVENT(PERSIST, ptcb- &tcbtab[0]));
       ptcb->tcb_ostate = TCPO_XMIT;
    }

    tcpkick(ptcb);      /* do something with it */
    return OK;
}
