#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  hglookup  -  get host group entry (if any) for a group
 * 	N.B. - Assumes HostGroup.hi_mutex *held*
 *------------------------------------------------------------------------
 */
struct hg* hglookup(unsigned int ifnum, IPaddr ipa) {
	struct hg  *phg;
	int i;

	phg = &hgtable[0];
	for (i=0; i < HG_TSIZE; ++i, ++phg) {
		if (phg->hg_state == HGS_FREE)
			continue;
		if (ifnum == phg->hg_ifnum && ipa == phg->hg_ipa)
			return phg;
	}
	return NULL;
}

