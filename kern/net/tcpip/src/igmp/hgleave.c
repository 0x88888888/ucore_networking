#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  hgleave  -  handle application request to leave a host group
 *------------------------------------------------------------------------
 */
int hgleave(unsigned int ifnum, IPaddr ipa) {
	struct hg  *phg;
	if (IP_CLASSD(ipa)){
		return SYSERR;
	}
	lock(&HostGroup.hi_mutex);
	if (!(phg=hglookup(ifnum, ipa)) || --(phg->hg_refs)) {
        unlock(&HostGroup.hi_mutex);
        return OK;
	}
	/* else, it exists & last reference */
	rtdel(ipa, ip_maskall);
	hgarpdel(ifnum, ipa);
	if (phg->hg_state == HGS_DELAYING) {
        panic("hgleave not be implemented fully\n");
		//tmclear(HostGroup.hi_uport, phg);
	}
	phg->hg_state = HGS_FREE;
	unlock(&HostGroup.hi_mutex);
    return OK;
}

