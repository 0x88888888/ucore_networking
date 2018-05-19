#include <tcpip/h/network.h>


IPaddr	ip_anyaddr = 0;
IPaddr	ip_loopback = 0x7F000001;

static uint32_t ipackid = 1;

/*------------------------------------------------------------------------
 *  ipsend  -  ip数据包进nif[NI_LOCAL].ni_ipinq队列,提醒ip_proc进程
 *------------------------------------------------------------------------
 */
int32_t ipsend(IPaddr faddr, struct ep* pep, int datalen,
              unsigned char proto, unsigned char ptos, unsigned ttl) {
	
    struct ip* pip = (struct ip*) pep->ep_data;
	pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
    pip->ip_tos = ptos;
    pip->ip_len = datalen+IP_HLEN(pip);
    pip->ip_id = getnext_ip_id();
    pip->ip_fragoff = 0;
    pip->ip_ttl = ttl;
    pip->ip_proto = proto;
    pip->ip_dst = faddr;
    //不必填写 pip->ip_src,ip_src在ipproc中去填写

    /*
	 * special case for ICMP, so source matches destination
	 * with multiple interfaces.
	 */
    if (pip->ip_proto != IPT_ICMP){
        //icmp协议数据，在icmp层就填好了,其余的协议在 ipproc中去填写
    	pip->ip_src = ip_anyaddr;
    } 
    //放到待处理ip数据包队列
    if(enq(nif[NI_LOCAL].ni_ipinq, pep)==false) {
       freebuf(pep);
       IpOutDiscards++;
       return SYSERR;
    }
    //send(ippid, NI_LOCAL);
    //ip_proc调用ipgetp接收
    ipc_event_send(ippid, EVT_TYP_IP_EVENT, NI_LOCAL);
    IpOutRequests++;
    return OK;
}

uint32_t
getnext_ip_id(){
    return ipackid++;
}

