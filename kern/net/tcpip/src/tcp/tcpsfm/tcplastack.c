#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcplastack -  do LAST_ACK state input processing
 *------------------------------------------------------------------------
 *  LASTACK状态
 *  被动关闭的一端，在发送FIN之后进入LAST_ACK状态
 */
int
tcplastack(struct tcb *ptcb, struct ep *pep) {
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	tcp	*ptcp = (struct tcp *)pip->ip_data;

	if (ptcp->tcp_code & TCPF_RST)
		return tcpabort(ptcb, TCPE_RESET);
	if (ptcp->tcp_code & TCPF_SYN) {
		//tcpreset马上发送 rst
		tcpreset(pep);
		return tcpabort(ptcb, TCPE_RESET);
	}
	tcpacked(ptcb, pep);
	if ((ptcb->tcb_code & TCPF_FIN) == 0)
		signal(&ptcb->tcb_ocsem);	/* close() deallocs	*/
	return OK;
}

