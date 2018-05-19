#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * arpsend - 广播一个ARP请求出去
 *------------------------------------------------------------------------
 */
int arpsend(struct arpentry* pae) {
	struct netif*   pni = pae->ae_pni;
	struct ep*      pep;
	struct arp*     parp;
    int             arplen;
    IPaddr  tmp=0;

    pep= (struct ep*)kmalloc(sizeof(struct ep));
    if (pep==NULL) {
    	return SYSERR;
    }
    // ep的地址为ff:ff:ff:ff:ff:ff
    memcpy(ep_dst(pep), pni->ni_hwb.ha_addr, pae->ae_hwlen);
    memcpy(ep_src(pep), pni->ni_hwa.ha_addr, pae->ae_hwlen);

    ep_type(pep) = EPT_ARP;
    parp = (struct arp*) pep->ep_data;
    parp->ar_hwtype = htons(pae->ae_hwtype);
    parp->ar_prtype = htons(pae->ae_prtype);
    parp->ar_hwlen = pae->ae_hwlen;
    parp->ar_prlen = pae->ae_prlen;
    parp->ar_op = htons(AR_REQUEST);

    memcpy(SHA(parp), pni->ni_hwa.ha_addr, pae->ae_hwlen);
    memcpy(SPA(parp), &pni->ni_ip, pae->ae_prlen);

    memset(THA(parp), 0, pae->ae_hwlen);
    tmp=*(IPaddr*)pae->ae_pra;
    tmp=htonl(tmp);
    memcpy(TPA(parp), &tmp, pae->ae_prlen);

    arplen = ARP_HLEN + 2*(parp->ar_hwlen + parp->ar_prlen); // 8+ 2*(6+4)
    //netwrite(pni, pep, EP_HLEN+arplen);
    net_dev_write(pni->ni_dev, pep, EP_HLEN+arplen); //20 + 
    return OK;
}
