#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpreset - 直接发送reset标记
 *------------------------------------------------------------------------
 * 发送 rst数据包
 *
 * tcpinp
 *   tcpclosed
 *      tcpreset
 *
 * tcpinp
 *    tcpreset
 *
 * tcpacked
 * tcpclosed
 * tcpclosewait
 * tcpclosing
 * tcpfin1
 * tcpfin2
 * tcplastack
 * tcplisten
 * tcpsynrcvd
 * tcpsynsent
 * tcptimewait
 * tcpestablished
 *    tcpreset
 *
 * 
 * 
 */
int
tcpreset(struct ep *pepin) {
	struct ep   *pepout;
	struct ip   *pipin = (struct ip*)pepin->ep_data;
	struct ip   *pipout;
	struct tcp  *ptcpin = (struct tcp *)pipin->ip_data;
	struct tcp  *ptcpout;
	int datalen;

    if (ptcpin->tcp_code & TCPF_RST) {
        // 对方已经rst过来了，就不会回复rst了，
        // 不然会永远的相互rst
    	return OK;   
    } 
    pepout = (struct ep*)kmalloc(sizeof(struct ep));
    if ((int)pepout == SYSERR) {
    	return SYSERR;
    }
    pipout = (struct ip *)pepout->ep_data;

    pipout->ip_src = pipin->ip_dst;
    pipout->ip_dst = pipin->ip_src;
    
    ptcpout = (struct tcp*)pipout->ip_data;
    ptcpout->tcp_sport = ptcpin->tcp_dport;
    ptcpout->tcp_dport = ptcpin->tcp_sport;

    if (ptcpin->tcp_code & TCPF_ACK) {
    	ptcpout->tcp_seq = ptcpin->tcp_ack;
    	ptcpout->tcp_code = TCPF_RST;
    } else {
    	ptcpout->tcp_seq = 0;
    	ptcpout->tcp_code = TCPF_RST | TCPF_ACK;
    }

    //就这样设置tcp_ack有bug，因为前面实际上并没有接收发过来的数据
    datalen = pipin->ip_len - IP_HLEN(pipin) - TCP_HLEN(ptcpin);
    if (ptcpin->tcp_code & TCPF_SYN) {
    	datalen++;
    }
    if (ptcpin->tcp_code & TCPF_FIN) {
    	datalen++;
    }
    
    //数据
    ptcpout->tcp_ack = ptcpin->tcp_seq + datalen;
    ptcpout->tcp_offset = TCPHOFFSET;
    ptcpout->tcp_window = ptcpout->tcp_urgptr = 0;
      
    tcph2net(ptcpout);
    // reset包，不带任何option数据
    ptcpout->tcp_cksum = 0;
    //ptcpout->tcp_cksum = tcpcksum2(pipout->ip_src, pipout->ip_dst, TCPMHLEN,  ptcpout);
    TcpOutSegs++;
    TcpOutRsts++;
    return ipsend(pipin->ip_src, pepout, TCPMHLEN, IPT_TCP, IPP_NORMAL, IP_TTL);
}

