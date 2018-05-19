#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpsynsent -  do SYN_SENT state processing
 *------------------------------------------------------------------------
 *
 *  客户端在发送syn标记后进入 SYN_SENT状态
 *  tcpinp
 *     tcpsynsent
 */
int
tcpsynsent(struct tcb *ptcb, struct ep *pep) {
    
    struct	ip	*pip = (struct ip *)pep->ep_data;
    struct	tcp	*ptcp = (struct tcp *)pip->ip_data;

    if ((ptcp->tcp_code & TCPF_ACK) &&
    	((ptcp->tcp_ack - ptcb->tcb_iss <=0) ||
    	  (ptcp->tcp_ack - ptcb->tcb_snext) > 0)) {
        /*ack序号不对，在SYN_SENT状态中， 
         *ack序号小于 初始序号或者大于希望收到的报文序号，都不对
         *tcpreset马上发送 rst
         */
    	return tcpreset(pep);
    }
    if (ptcp->tcp_code & TCPF_RST) {
    	ptcb->tcb_state = TCPS_CLOSED;
    	ptcb->tcb_error = TCPE_RESET;
    	TcpAttemptFails++;
    	tcpkilltimers(ptcb);
    	signal(&(ptcb->tcb_ocsem));
    	return OK;
    }
    if ((ptcp->tcp_code & TCPF_SYN) == 0) {
        //SYN_SENT状态中,收到没有SYN标记的报文，直接跳过了
    	return OK;
    }
    
    //记录对方的tcp_window
    ptcb->tcb_swindow = ptcp->tcp_window;
    //
    ptcb->tcb_lwseq = ptcp->tcp_seq;
    //记录下次希望接收到的报文序号
    ptcb->tcb_rnext = ptcp->tcp_seq;
    
    //本端可接受报文的最大序号
    ptcb->tcb_cwin = ptcb->tcb_rnext + ptcb->tcb_rbsize;
    /*如果报文只含有 SYN（不含ack）,tcpacked就不做处理，否则tcpacked就清除报文段的syn标记*/
    tcpacked(ptcb, pep);
    tcpdata(ptcb, pep);
    ptcp->tcp_code &= ~TCPF_FIN;
    if (ptcb->tcb_code & TCPF_SYN) { /* our SYN not ACKed	*/
       //如果syn没有被tcpacked清除掉，说明收到的是一个syn标记报文段
       // 此时连接状态转到 SYNRCVD

       /* 发出去的syn没有被确认 */
       ptcb->tcb_state = TCPS_SYNRCVD;
    } else {
        /* pep 是一个syn + ack 的报文段，切换到 TCPS_ESTABLISHED*/
    	TcpCurrEstab++;
    	ptcb->tcb_state = TCPS_ESTABLISHED;
    	signal(&(ptcb->tcb_ocsem)); /* return in open	*/
    }
    return OK;
}


