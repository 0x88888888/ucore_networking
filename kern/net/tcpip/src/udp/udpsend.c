#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  udpsend  -  send one UDP datagram to a given IP address
 *------------------------------------------------------------------------
 */
int udpsend(IPaddr fip, unsigned short fport, unsigned short lport,
	       struct ep* pep, unsigned datalen, bool do_cksum) {

    struct ip*   pip  = (struct ip*) pep->ep_data;
    struct udp*  pudp = (struct udp*)pip->ip_data;
    struct route *prt;
    struct  hg  *phg;
    int     ttl = IP_TTL;

    //pip->ip_proto = IPT_UDP;
    pudp->u_src = lport;
    pudp->u_dst = fport;
    pudp->u_len = U_HLEN + datalen;
    pudp->u_cksum = 0;
    udph2net(pudp);
    
    /*
    if (do_cksum) {
        
        //本地产生数据，发送到fip
    	prt = rtget(fip, RTF_LOCAL);
    	if (prt == NULL) {
    		IpOutNoRoutes++;
			freebuf(pep);
			return SYSERR;
    	}
    	pip->ip_dst= fip;
    	if (prt->rt_ifnum == NI_LOCAL){
            //本地接口127.0.0.1,发送端ip和接受端ip一样
			pip->ip_src= pip->ip_dst;
    	}
		else{
            //另外的出口
			pip->ip_src=  nif[prt->rt_ifnum].ni_ip;
		}
		rtfree(prt);
		pudp->u_cksum = udpcksum(pep,  U_HLEN+datalen);
		if (pudp->u_cksum == 0){
			pudp->u_cksum = ~0;
		}
    }
    */
    UdpOutDatagrams++;
    if (IP_CLASSD(fip)) {
        //组播
        lock(&HostGroup.hi_mutex);
        phg=hglookup(NI_PRIMARY, fip);
        if (phg) {
            ttl = phg->hg_ttl;
        } else {
            ttl = 1;
        }
        unlock(&HostGroup.hi_mutex);
    	//panic("in udpsend: about igmp not be implemented\n");
    }
    return ipsend(fip, pep, U_HLEN+datalen, IPT_UDP, IPP_NORMAL, ttl);
}


