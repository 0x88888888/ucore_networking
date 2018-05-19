#include <tcpip/h/network.h>

mutex udpmutex;

/*------------------------------------------------------------------------
 *  udp_in -  handle an inbound UDP datagram
 *------------------------------------------------------------------------
 */
int udp_in(struct netif* pni, struct ep* pep) {
	struct ip*   pip =(struct ip* )pep->ep_data;
	struct udp*  pudp=(struct udp*)pip->ip_data;
	struct upq *pup;
    unsigned short dst;
    int   i;
    int len = pip->ip_len - IP_HLEN(pip);

    //if (pudp->u_cksum && udpcksum(pep, ntohs(pudp->u_len))) {
    if (pudp->u_cksum && tcpcksum2( pip->ip_src, pip->ip_dst, IPT_UDP, len, pudp)){
        cprintf("udp_in 0\n");
    	freebuf(pep);
    	return SYSERR;    /* checksum error */
    }
    cprintf("udp_in 1\n");
    udpnet2h(pudp);   /* convert UDP header to host order */
    dst = pudp->u_dst;
    lock(&udpmutex);
    for(i=0; i< UPPS; i++) {
    	pup = &upqs[i];
    	if (pup->up_port == dst) {
            cprintf("udp_in 2\n");
    		// drop instead of blocking on psend ,只能有UPPLEN个消息可以在up_xport队列中
            if (pcount(pup->up_xport) >= UPPLEN) {
                unlock(&udpmutex);
                freebuf(pep);
                UdpInErrors++;
                return SYSERR;
            }
            //通知掉
            psend(pup->up_xport, (int)pep);
            UdpInDatagrams++;
            if (!isbadpid(pup->up_pid)) {
                //通知来源进程，udp模块已经处理?
                ipc_event_send(pup->up_pid, EVT_TYP_UDP_EVENT, 0 /*msg*/);
                pup->up_pid = BADPID;
            }
            unlock(&udpmutex);
            return OK;
    	}
    }
    unlock(&udpmutex);
    UdpNoPorts++;
    icmp(ICT_DESTUR, ICC_PORTUR, pip->ip_src, pep, 0);
    return OK;
}