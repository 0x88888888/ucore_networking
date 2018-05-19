#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * ipputp  -  send a packet to an interface's output queue
 *            将数据包pep通过nif[inum]直接发送或者发送到nexthop
 *------------------------------------------------------------------------
 */
int ipputp(int inum, IPaddr nexthop, struct ep* pep) {
	struct netif   *pni = &nif[inum];
	struct ip*     pip;
	int  hlen /*最后一个分片的数据长度*/;
    int maxdlen, tosend, offset /*已经发送的数据量*/, offindg/*下一个要发送的分片的偏移*/;
 
	if (pni->ni_state == NIS_DOWN) {
        freebuf(pep);
        return SYSERR;
	}
    ep_type(pep) = EPT_IP;
    /* todo
     * 在这里需要对 tcp,udp数据专门计算check sum
     *
     */


	pip = (struct ip*)pep->ep_data;
    
    struct tcp* ptcp = (struct tcp*) ((char*)pip + IP_HLEN(pip));
    ptcp->tcp_cksum = tcpcksum2( pip->ip_src, pip->ip_dst,pip->ip_proto,
                               (pip->ip_len - IP_HLEN(pip)), ptcp  );

	if (pip->ip_len <= pni->ni_mtu) {
        //直接发送,不需要分片
		pep->ep_nexthop = nexthop;
		pip->ip_cksum = 0;
		iph2net(pip);
        pip->ip_cksum = cksum((unsigned short*)pip, IP_HLEN(pip)/2);
        return netwrite(pni, pep, EP_HLEN+ntohs(pip->ip_len));
	}

    //走到这里就说明数据包太大，需要分片发送
  
    if (pip->ip_fragoff & IP_DF) {
    	//不允许fragment,但是又超过了mtu,icmp 通知到 ip_src
    	icmp(ICT_DESTUR, ICC_FNADF, pip->ip_src, pep, 0);
    	return OK;
    }
    /*
     * (pni->ni_mtu - IP_HLEN(pip))& 0xffffff00
     *  最大的数据长度
     */
    maxdlen = (pni->ni_mtu - IP_HLEN(pip)) &(~7);
    offset = 0; 
    //offindg 填到 fragoff中,表示在原始ip数据报中的偏移
    offindg = (pip->ip_fragoff & IP_FRAGOFF) << 3;
    tosend = pip->ip_len - IP_HLEN(pip); //需要发送的数据大小
    while(tosend > maxdlen) {
    	if(ipfsend(pni, nexthop, pep, offset, maxdlen, offindg) != OK) {
           IpOutDiscards++;
           freebuf(pep);
           return SYSERR;
    	}
    	IpFragCreates++;
    	tosend -= maxdlen;
    	offset += maxdlen;
    	offindg += maxdlen;
    }
    //最后一个包
    IpFragOKs++;
    IpFragCreates++;
    //原ip头信息随着最后一个包带走
    hlen = ipfhcopy(pep, pep, offindg);
    pip = (struct ip*)pep->ep_data;
    /* slide the residual down */
    blkcopy(&pep->ep_data[hlen], &pep->ep_data[IP_HLEN(pip)+offset], tosend);
    
    /* 设置frag_offset ,并且keep MF, if this was a frag to start with */
    pip->ip_fragoff = (pip->ip_fragoff & IP_MF) | (offindg >> 3);
    pip->ip_len = tosend + hlen;
    pip->ip_cksum = 0;
    iph2net(pip);
    pip->ip_cksum = cksum((unsigned short*)pip, hlen>>1);
    pep->ep_nexthop = nexthop;
    return netwrite(pni, pep, EP_HLEN+ntohs(pip->ip_len));
}


