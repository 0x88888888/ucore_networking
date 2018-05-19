#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  local_out  -  handle an IP datagram headed for a local process
 *------------------------------------------------------------------------
 *
 * netwrite
 *    local_out
 *
 */
int local_out(struct ep* pep) {
    struct netif   *pni = &nif[NI_LOCAL];
    struct ip      *pip = (struct ip*)pep->ep_data;
    int  rv;

    ipnet2h(pip);

    pep = ipreass(pep);
    if (pep == 0) {
    	return OK;
    }
    pip = (struct ip*)pep->ep_data;
    ipdstopts(pni, pep);

    switch (pip->ip_proto) {
        case IPT_ICMP:
            rv = icmp_in(pni, pep);
            break;
        case IPT_IGMP:
            rv = igmp_in(pni, pep);
            break;
        case IPT_TCP:
            rv = tcp_in(pni, pep);
            break;
        case IPT_UDP:
            rv = udp_in(pni, pep);
            break;
        default:
            IpInUnknownProtos++;
            icmp(ICT_DESTUR, ICC_PROTOUR, pip->ip_src, pep, 0);
            return OK;
    }
    IpInDelivers++;
    return rv; 
}
