#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpclosing -  do CLOSING state input processing
 *                
 *  在FIN_WAIT_1状态收到对方发过来的FIN标记，而不是SYN或者SYN+ACK标记，
 *  就由FIN_WAIT_1进入 CLOSING状态
 *  这个情况，可以理解成，两端同时发出FIN标记，
 *  两端在各自接受到一个ACK之后，就都进入了TIME_WAIT状态了
 *                
 *------------------------------------------------------------------------
 */
int
tcpclosing(struct tcb *ptcb, struct ep *pep)
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	tcp	*ptcp = (struct tcp *)pip->ip_data;
    
    if (ptcp->tcp_code & TCPF_RST) {
    	return tcbdealloc(ptcb);
    }
    if (ptcp->tcp_code & TCPF_SYN) {
    	//tcprest马上发送 rst
		tcpreset(pep);
		return tcbdealloc(ptcb);
	}
	tcpacked(ptcb, pep);
	if((ptcb->tcb_code & TCPF_FIN) == 0) {
		//CLOSING状态不再接受FIN标记了
		ptcb->tcb_state = TCPS_TIMEWAIT;
		signal(&(ptcb->tcb_ocsem));
		tcpwait(ptcb);
	}
	return OK;
}

  