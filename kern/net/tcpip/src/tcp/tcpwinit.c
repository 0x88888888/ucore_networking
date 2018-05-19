#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpwinit - 从父ptcb复制信息到newptcb
 *------------------------------------------------------------------------
 *
 *tcpinp
 *   tcplisten
 *      tcpwinit
 */
int
tcpwinit(struct tcb *ptcb /*父tcb*/, struct tcb *newptcb /*子tcb*/, struct ep *pep) {

	struct ip   *pip  = (struct ip*)pep->ep_data;
	struct tcp  *ptcp = (struct tcp*)pip->ip_data;
    struct route *prt;
    unsigned int mss;
    bool         local;

    newptcb->tcb_swindow= ptcp->tcp_window;
    newptcb->tcb_lwseq = ptcp->tcp_seq;
    newptcb->tcb_lwack = newptcb->tcb_iss; 

    prt = (struct route*)rtget(pip->ip_src, RTF_REMOTE);
    local = prt && prt->rt_metric == 0;
    newptcb->tcb_pni = &nif[prt->rt_ifnum];
    rtfree(prt);
    if(local) {
        //在同一个网络中
    	mss = newptcb->tcb_pni->ni_mtu - IPMHLEN - TCPMHLEN;
    } else {
    	//不在同一个网络中的
    	mss = 536;
    }

    if (ptcb->tcb_smss) {
    	newptcb->tcb_smss = min(ptcb->tcb_smss, mss);
        //重新设置listen的tcb smss
    	ptcb->tcb_smss = 0; /* reset server smss	*/
    } else {
        
    	newptcb->tcb_smss = mss;
    }
    
    newptcb->tcb_rmss = mss;   /* receive mss		*/
    //拥塞窗口值
    newptcb->tcb_cwnd = newptcb->tcb_smss;  /* 1 segment */
    newptcb->tcb_ssthresh = 65535; /* IP max window	*/
    //希望接收的下一个序号值
    newptcb->tcb_rnext = ptcp->tcp_seq;
    //接受窗口的最大序号值
    newptcb->tcb_cwin = newptcb->tcb_rnext + newptcb->tcb_rbsize;
    return OK;
}


