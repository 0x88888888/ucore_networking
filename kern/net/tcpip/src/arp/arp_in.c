#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  arp_in  -  只处理网卡进来的ARP请求
 *-----------------------------------------------------------------------
 *
 * ni_in
 *     arp_in
 *
 */
int arp_in(struct netif *pni, struct ep *pep) {
    struct arp*        parp=(struct arp*)pep->ep_data;
    struct arpentry*   pae;
    int arplen;
    parp->ar_hwtype = ntohs(parp->ar_hwtype);
    parp->ar_prtype = ntohs(parp->ar_prtype);//0800
    parp->ar_op = ntohs(parp->ar_op);
    int tmpip;

    if (parp->ar_hwtype != pni->ni_hwtype ||
    	parp->ar_prtype != EPT_IP) {
        //不是本网卡接口的硬件地址，协议地址类型不是 IPV4
    	freebuf(pep);
        return OK;
    }
    
    //得到ARP表的缓存,不判断ar_op了，默认看作reply，处于ae_pending的，也直接修改了mac addr
    //tmpip=ntohl(*(SPA(parp)));
    tmpip= ntohl( *(IPaddr*)(SPA(parp)) );
    if( (pae= arpfind(&tmpip, parp->ar_prtype, pni)) ) {
        //更新TTL值
        blkcopy(pae->ae_hwa, SHA(parp), pae->ae_hwlen);
        pae->ae_ttl = ARP_TIMEOUT;
        arpprint();
    }
    if (!blkequ(TPA(parp), &pni->ni_ip, IP_ALEN)) {
        //不是发送给本网卡接口的的ip,丢弃
        freebuf(pep);
        return OK;
    }
    if (pae == NULL){
        // 在arptable中添加一个 item，存储对方的ARP信息
        pae = arpadd(pni, parp);
    }
    if (pae->ae_state == AS_PENDING) {
        
        //得到对方的arp信息，修改状态
        pae->ae_state = AS_RESOLVED;
        arpprint();
        arpqsend(pae);//状态修改到 AS_RESOLVED之后，处理缓存在ae_queue中的包
    }

    if (parp->ar_op == AR_REQUEST) {
        //回复 ARP请求
        parp->ar_op = AR_REPLY;
        blkcopy(TPA(parp), SPA(parp), parp->ar_prlen);
        blkcopy(THA(parp), SHA(parp), parp->ar_hwlen);
        blkcopy(ep_dst(pep), THA(parp), EP_ALEN);
        blkcopy(SHA(parp), pni->ni_hwa.ha_addr, pni->ni_hwa.ha_len);
        blkcopy(SPA(parp), &pni->ni_ip, IP_ALEN);

        parp->ar_hwtype = htons(parp->ar_hwtype);
        parp->ar_prtype = htons(parp->ar_prtype);
        parp->ar_op = htons(parp->ar_op);

        arplen = sizeof(struct arp) + 2*(parp->ar_prlen + parp->ar_hwlen);
        //netwrite(pni, pep, arplen);
        net_dev_write(pni->ni_dev, pep, arplen);
    } else {
        freebuf(pep);
    }
    
    return  OK;
}

