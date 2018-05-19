#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  igmp_in  -  handle IGMP packet coming in from the network
 *------------------------------------------------------------------------
 */
int igmp_in(struct netif* pni, struct ep *pep) {
	struct ip*     pip;
	struct igmp*   pig;
	struct hg*     phg;
	int len;

	pip = (struct ip*) pep->ep_data;
	pig = (struct igmp*) pip->ip_data;

	len = pip->ip_len - IP_HLEN(pip);
	if (len != IG_HLEN || IG_VER(pig) != IG_VERSION ||
		cksum((unsigned short*)pig, len>>1)) {
       freebuf(pep);
       return SYSERR;
	}

	switch(IG_TYP(pig)) {
		case IGT_HQUERY:
		    panic("igmp_in not be implemented fully\n");
		    //igmp_settimers(NI_PRIMARY);
		    break;
		case IGT_HREPORT:
            lock(&HostGroup.hi_mutex);
            if ((phg = hglookup(NI_PRIMARY, pig->ig_gaddr)) &&
            	phg->hg_state == HGS_DELAYING) {
            	panic("igmp_in not be implemented fully\n");
            	//tmclear(&HostGroup.hi_uport, phg);
                phg->hg_state = HGS_IDLE;
            }
            unlock(&HostGroup.hi_mutex);
		    break;
		default:
		    break;
	}
    freebuf(pep);
    return OK;
}

