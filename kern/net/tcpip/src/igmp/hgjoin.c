#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  hgjoin  -  handle application request to join a host group
 *------------------------------------------------------------------------
 */
 int hgjoin(unsigned int ifnum, IPaddr ipa, bool islocal) {
 	struct hg *phg;
 	int i;
    
    //必须是组播地址224.
 	if (!IP_CLASSD(ipa)) {
 		return SYSERR;
 	}
 	/* restrict multicast in multi-homed host to primary interface */
 	if (ifnum != NI_PRIMARY) {
 		return SYSERR;
 	}
 	lock(&HostGroup.hi_mutex);
    if ( (phg = hglookup(ifnum, ipa)) ) {
    	phg->hg_refs++;
    	unlock(&HostGroup.hi_mutex);
    	return OK;
    }
    unlock(&HostGroup.hi_mutex);
    /* add to host group and routing tables */
    if (hgadd(ifnum, ipa, islocal) == SYSERR)
       return SYSERR;
               /*255.255.255.255*/
    rtadd(ipa, ip_maskall, ipa, 0, NI_LOCAL, RT_INF);
    /*
	 * advertise membership to multicast router(s); don't advertise
	 * 224.0.0.1 (all multicast hosts) membership.
	 */
    if (ipa != ig_allhosts){
        for (i=0; i< IG_NSEND; i++) {
        	igmp(IGT_HREPORT, ifnum, ipa);
        	do_sleep(IG_DELAY);
        }
    }
    return OK;
 }

