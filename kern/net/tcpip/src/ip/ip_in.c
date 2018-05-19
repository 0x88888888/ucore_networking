#include <tcpip/h/network.h>

int ip_in(struct netif *pni, struct ep *pep){
    
    struct ip* pip = pep->ep_data;
    int sum = 0;
    int ip_len = ntohs(pip->ip_len);

    IpInReceives++;
    
    if (enq(pni->ni_ipinq, pep) == false) {
    	IpInDiscards++;
    	freebuf(pep);
    }
    //发到ipproc
    ipc_event_send(ippid, EVT_TYP_IP_EVENT, pni->ni_num );
    
    return OK;
    
}

