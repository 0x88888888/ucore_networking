#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ipfsend -  send one fragment of an IP datagram
 *             发送一个数据包片
 *             从pep->ep_data->ip_data+offset开始 复制 maxdlen个字节，发送出去
 *------------------------------------------------------------------------
 */
int ipfsend(struct netif* pni, IPaddr nexthop, struct ep* pep, 
	       int offset, int maxdlen, int offindg) {
	struct ep*   pepnew;
	struct ip*   pip;
	struct ip*   pipnew;
	int    hlen, len;

	pepnew = (struct ep*)kmalloc(sizeof(struct ep));
	if (pepnew == NULL) {
       return SYSERR;
	}
    //复制原始header
    hlen = ipfhcopy(pepnew, pep, offindg);  /* copy the headers */
    
    //修改header信息
    pip = (struct ip*)pep->ep_data;
    pipnew = (struct ip*)pepnew->ep_data;
    pipnew->ip_fragoff = IP_MF | (offindg>>3); //设置 more fragment
    pipnew->ip_len = len = maxdlen + hlen;
    pipnew->ip_cksum = 0;

    iph2net(pipnew);
    pipnew->ip_cksum = cksum((unsigned short*)pipnew, hlen>>1);
    
    blkcopy(&pepnew->ep_data[hlen], &pep->ep_data[IP_HLEN(pip)+offset], maxdlen);
    pepnew->ep_nexthop = nexthop;
   
    
    return netwrite(pni, pepnew, EP_HLEN+len);
}
