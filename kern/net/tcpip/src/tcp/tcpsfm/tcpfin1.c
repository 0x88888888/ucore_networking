#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpfin1 -  do FIN_WAIT_1 state input processing
 *------------------------------------------------------------------------
 *
 * 在 ESTABLISH状态中，收到一个FIN,就会进入 FIN_WAIT_1状态
 *
 */
int
tcpfin1(struct tcb *ptcb, struct ep *pep) {
	struct	ip	*pip	= (struct ip *)pep->ep_data;
	struct	tcp	*ptcp	= (struct tcp *)pip->ip_data;

	if (ptcp->tcp_code & TCPF_RST)
		return tcpabort(ptcb, TCPE_RESET);
	if (ptcp->tcp_code & TCPF_SYN) {
		//tcprest马上发送 rst
		tcpreset(pep);
		return tcpabort(ptcb, TCPE_RESET);
	}

	//预处理
	if (tcpacked(ptcb, pep) == SYSERR){
		return OK;
	}

	tcpdata(ptcb, pep);
	tcpswindow(ptcb, pep);

	if (ptcb->tcb_flags & TCBF_RDONE) {
		//RDONE，说明在FIN1状态接收到对方已经发送了的FIN了
		if (ptcb->tcb_code & TCPF_FIN) { /* FIN not ACKed*/
		    // 如果TCPF_FIN没有被清除，说明也是刚刚发送出去FIN,
		    // 没有收到ACK会议，则双方同时关闭
            ptcb->tcb_state = TCPS_CLOSING;
		} else {
			ptcb->tcb_state = TCPS_TIMEWAIT;
			signal(&(ptcb->tcb_ocsem));
			tcpwait(ptcb);
		}
	} else if ( (ptcb->tcb_code & TCPF_FIN) == 0){
		 //没有FIN，只有ACK标记，进入 FIN_WAIT_2 状态
        signal(&(ptcb->tcb_ocsem));   /* wake closer	*/
        ptcb->tcb_state = TCPS_FINWAIT2;
	}
	return OK;
}

