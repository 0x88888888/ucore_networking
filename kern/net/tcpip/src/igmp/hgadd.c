#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  hgadd  -  add a host group entry for a group
 *------------------------------------------------------------------------
 */
int hgadd(unsigned ifnum, IPaddr ipa, bool islocal) {
	struct hg   *phg;
	static int   start;
	int     i;

	lock(&HostGroup.hi_mutex);
	for (i=0; i< HG_TSIZE; i++) {
		if (++start >= HG_TSIZE) {
			start =0;
		}
		if (hgtable[start].hg_state == HGS_FREE) {
			break;
		}
	}
	phg = &hgtable[start];
    if (phg->hg_state != HGS_FREE) {
    	unlock(&HostGroup.hi_mutex);
    	return SYSERR;  /* table full */
    }
    if (hgarpadd(ifnum, ipa) == SYSERR) {
        unlock(&HostGroup.hi_mutex);
        return SYSERR;
	}
	phg->hg_ifnum = ifnum;
	phg->hg_refs = 1;
	if (islocal) {
		phg->hg_ttl = 1;
	} else {
		phg->hg_ttl = IP_TTL;
	}
	//blkcopy(phg->hg_ipa, ipa, IP_ALEN);
	phg->hg_ipa = ipa;
	if (ipa == ig_allhosts) {
		phg->hg_state = HGS_STATIC;
	} else {
		phg->hg_state = HGS_IDLE;
	}
	unlock(&HostGroup.hi_mutex);
	return OK;
}

