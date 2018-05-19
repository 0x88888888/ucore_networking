#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpackit -  
 *              只发送一个ack回复
 *------------------------------------------------------------------------
 *
 * tcpinp
 *    tcpackit
 */
int
tcpackit(struct tcb *ptcb, struct ep *pepin) {
	struct ep *pepout;
	struct ip *pipin = (struct ip *)pepin->ep_data;
	struct ip *pipout;
	struct tcp *ptcpin = (struct tcp *)pipin->ip_data;
	struct tcp *ptcpout;

	if (ptcpin->tcp_code & TCPF_RST) {
       return OK;
	}
	if (pipin->ip_len <= IP_HLEN(pipin) + TCP_HLEN(ptcpin) &&
		!(ptcpin->tcp_code & (TCPF_SYN|TCPF_FIN))) {
		return OK;  /* 重复ACK了 */
	}
    pepout = (struct ep *)kmalloc(sizeof(struct ep));
    if (pepout == NULL) {
    	return SYSERR;
    }
    pipout = (struct ip*)pepout->ep_data;
    pipout->ip_src=pipin->ip_dst;
    pipout->ip_dst=pipin->ip_src;
    //memcpy(pipout->ip_src, pipin->ip_dst, IP_ALEN);
    //memcpy(pipout->ip_dst, pipin->ip_src, IP_ALEN);

    ptcpout = (struct tcp*)pipout->ip_data;
    ptcpout->tcp_sport = ptcpin->tcp_dport;
    ptcpout->tcp_dport = ptcpin->tcp_sport;
    ptcpout->tcp_seq = ptcb->tcb_snext;
    ptcpout->tcp_ack = ptcb->tcb_rnext;

    ptcpout->tcp_code = TCPF_ACK;
    ptcpout->tcp_offset = TCPHOFFSET;
    ptcpout->tcp_window = tcprwindow(ptcb);
    ptcpout->tcp_cksum = 0;
    ptcpout->tcp_urgptr = 0;

    tcph2net(ptcpout);
    ptcpout->tcp_cksum = 0;
    //ack包，不带option数据
    //ptcpout->tcp_cksum = tcpcksum2(htonl(ptcb->tcb_lip), htonl(ptcb->tcb_rip), TCPMHLEN,ptcpout);

    TcpOutSegs++;
    return ipsend(pipin->ip_src, pepout, TCPMHLEN, IPT_TCP,
    	IPP_NORMAL, IP_TTL);
}

