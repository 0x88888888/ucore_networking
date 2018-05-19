#include <tcpip/h/network.h>

int ni_in(struct netif *pni,/* the interface*/
          struct ep *pep,   /* the packet */
          int        len    /* length, in octets*/)
{
    int rv;
    char* bufrecv = (char*) pep;
    int idx;

    pni->ni_ioctets += len;
    if(blkequ(pni->ni_hwa.ha_addr, ep_dst(pep), EP_ALEN)){
        pni->ni_iucast++;
    } else {
        pni->ni_inucast++;
    }


    ep_type(pep) = ntohs(ep_type(pep));

    switch(ep_type(pep)) {
    	case EPT_ARP:
    	     rv = arp_in(pni, pep);
    	     break;
    	case EPT_RARP:
    	     rv = rarp_in(pni, pep);
    	     break;
    	case EPT_IP:
    	     rv = ip_in(pni, pep);
    	     break;
    	default:
    	   pni->ni_iunkproto++;
    	   freebuf(pep);
    	   return OK;
    }

    return rv;
}

