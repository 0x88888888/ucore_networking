#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpestablished -  do ESTABLISHED state input processing
 *------------------------------------------------------------------------
 */
int
tcpestablished(struct tcb *ptcb, struct ep *pep) {
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	tcp	*ptcp = (struct tcp *)pip->ip_data;
   
    //ESTABLISHED状态遇到rst,syn， 就重置了，回收当前的连接
	if (ptcp->tcp_code & TCPF_RST) {
		TcpEstabResets++;
		TcpCurrEstab--;
		//清理ptcb工作
		return tcpabort(ptcb, TCPE_RESET);
	}
	if (ptcp->tcp_code & TCPF_SYN) {
		TcpEstabResets++;
		TcpCurrEstab--;
		//tcpreset 马上发送 rst
		tcpreset(pep);
		return tcpabort(ptcb, TCPE_RESET);
	}

	//处理ack
	if (tcpacked(ptcb, pep) == SYSERR) {
		return OK;
	}
	//接受数据
	tcpdata(ptcb, pep);

	tcpswindow(ptcb, pep);
	
	if (ptcb->tcb_flags & TCBF_RDONE){
		//停止读数据
		ptcb->tcb_state = TCPS_CLOSEWAIT;
	}
	return OK;
}

