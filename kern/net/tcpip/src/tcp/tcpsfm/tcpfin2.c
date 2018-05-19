#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpfin2 -  do FIN_WAIT_2 state input processing
 *------------------------------------------------------------------------
 */
int
tcpfin2(struct tcb *ptcb, struct ep *pep)
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
    struct	tcp	*ptcp = (struct tcp *)pip->ip_data;

    if (ptcp->tcp_code & TCPF_RST) {
    	return tcpabort(ptcb, TCPE_RESET);
    }
    if (ptcp->tcp_code & TCPF_SYN) {
        //tcpreset马上发送 rst
    	tcpreset(pep);
    	return tcpabort(ptcb, TCPE_RESET);
    }
    if (tcpacked(ptcb, pep)== SYSERR) {
    	return OK;
    }
    tcpdata(ptcb, pep); /* for data + FIN ACKing */

    if (ptcb->tcb_flags & TCBF_RDONE) {
        //不再读取数据了，切换状态到TCPS_TIMEWAIT
    	ptcb->tcb_state = TCPS_TIMEWAIT;
    	tcpwait(ptcb);
    }
    return OK;
}

