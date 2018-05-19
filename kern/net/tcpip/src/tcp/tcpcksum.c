#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpcksum -  compute a TCP pseudo-header checksum
 *
 *  saddr和daddr为网络序
 *  ptcp中的tcp头信息已经为网络序
 *------------------------------------------------------------------------
 *
 */
unsigned short
tcpcksum(struct ep* pep, IPaddr saddr, IPaddr daddr ) {
    uint32_t sum = 0;
    uint32_t ret =0;
    sum += saddr; //4
    sum += daddr; //4
    sum += htons(IPT_TCP); // 2
    struct ip*  pip  = (struct ip*)pep->ep_data;
    struct tcp* ptcp = (struct tcp*) pip->ip_data;
    int len = pip->ip_len - IP_HLEN(pip);
    
    unsigned char* p1 = (unsigned char*)ptcp;
    cprintf("\n tcpcksum 1----------------------\n");
    cprintf("ip_len = %x \n",pip->ip_len);
    cprintf("IP_HLEN =%x \n",IP_HLEN(pip));
    for(ret = 0 ; ret< len; ret++) {
        if (ret % 12 == 0) {
            cprintf("\n");
        }
        cprintf("0x%02x, ",p1[ret]);
    }
    cprintf("\ntcpcksum 2----------------------\n");

    sum += htons(len);

    /*
    int tcpheadlen = TCP_HLEN(ptcp);// (ptcp->tcp_offset &0xff ) * 4;
    int tcpdatalen = pip->ip_len - IP_HLEN(pip) - TCP_HLEN(ptcp);
    sum += htons(tcpdatalen); // 
    */
    

    //char* data = ((char*)ptcp) - tcpheadlen;
	ret = calculate_checksum(ptcp,len, sum);
	cprintf("saddr =%x \n", saddr);
	cprintf("daddr =%x \n", daddr);
	cprintf("htons(IPT_TCP) = %x\n", htons(IPT_TCP));
    cprintf("len = %d \n", len);
    cprintf("sum= %x\n",sum);
    cprintf("ret= %x\n",ret);
	return ret;
}


/*
 * saddr, daddr,tcplen都是主机序, 
 * tcplen 表示 ptcp的字节数量
 *
 */
unsigned short 
tcpcksum2(IPaddr saddr, IPaddr daddr, unsigned short proto ,uint16_t tcplen, void* pudp_or_tcp) {
    uint32_t sum = 0;

    sum += htonl(saddr);   //4
    sum += htonl(daddr);   //4
    //sum += htons(IPT_TCP); // 2
    sum += htons(proto);
    sum += htons(tcplen);  // 2
    
    cprintf("tcpcksum2 saddr = %x \n", saddr);
    cprintf("tcpcksum2 daddr = %x \n", daddr);
    cprintf("tcpcksum2 tcplen = %d \n", tcplen);
    cprintf("tcpcksum2 proto = %04x  htons(proto) = %04x\n", proto, htons(proto));

    sum = calculate_checksum(pudp_or_tcp,tcplen, sum);
    cprintf("check sum = %x\n",sum);

    
    return sum;
}

