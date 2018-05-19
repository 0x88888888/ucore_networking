#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpsynrcvd -  do SYN_RCVD state input processing
 *------------------------------------------------------------------------
 *
 * LISTEN状态时，收到一个syn 报文段，就进入 SYN_RCVD状态
 * tcpinp
 *    tcpsynrcvd
 */
int
tcpsynrcvd(struct tcb *ptcb, struct ep *pep) {
	struct ip  *pip  = (struct ip*) pep->ep_data;
	struct tcp *ptcp = (struct tcp*) pip->ip_data;
    struct  tcb *pptcb;

    if (ptcp->tcp_code & TCPF_RST){
    	TcpAttemptFails++;
    	if (ptcb->tcb_pptcb != 0) {
            //客户端发过来一个rst
    		//服务端的处理方式,这个tcb是从 listen tcb分化出来的
    		return tcbdealloc(ptcb);
    	} else {
            //服务端发过来的一个rst
    		//客户端的处理方式
    		return tcpabort(ptcb, TCPE_REFUSED);
    	}
    }
    
    //只等待 ack
    if (ptcp->tcp_code & TCPF_SYN) {
      //已经接收到过syn了，握手协议就不再接收syn标记了 
    	TcpAttemptFails++;
        //tcpreset马上发送rst
    	tcpreset(pep);
    	return tcpabort(ptcb, TCPE_RESET);
    }
    
    //tcpacked 处理ack报文段，如果不是ack报文段，就返回 syserr
    if (tcpacked(ptcb, pep) == SYSERR){
    	return OK;
    }

    if (ptcb->tcb_pptcb != 0) { /* from a passive open	*/
       //服务端，被动打开的连接
        pptcb = ptcb->tcb_pptcb;
        lock(&(pptcb->tcb_mutex));

        if (pptcb->tcb_state != TCPS_LISTEN) {
            //父tcb必须是 LISTEN 的tcb
        	TcpAttemptFails++;
            //tcpreset马上发送 rst
        	tcpreset(pep);
        	signal(&(pptcb->tcb_mutex));
        	return tcbdealloc(ptcb);
        }
        

        if (pcount(pptcb->tcb_listenq) >= pptcb->tcb_lqsize) {
            //父监听队列已经满了
        	TcpAttemptFails++;
            //tcpreset马上发送 rst
        	tcpreset(pep);
        	signal(&(pptcb->tcb_mutex));
        	return tcbdealloc(ptcb);
        }
        //连接完成，通知父tcb,将改连接放入listenq 队列
        psend(pptcb->tcb_listenq, ptcb->tcb_dvnum);
        unlock(&(pptcb->tcb_mutex));
    } else {
    	// from an active open
    	//客户端，主动打开
    	signal(&(ptcb->tcb_ocsem));
    }
    TcpCurrEstab++;
    ptcb->tcb_state = TCPS_ESTABLISHED;
    tcpdata(ptcb, pep);
    if (ptcb->tcb_flags & TCBF_RDONE) {
        //如果收到FIN标记
    	ptcb->tcb_state = TCPS_CLOSEWAIT;
    }
    return OK;
}

