#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  igmp_settimers  -  generate timer events to send IGMP reports
 *------------------------------------------------------------------------
 */
int igmp_settimers(unsigned int ifnum) {
	int i;
	lock(&HostGroup.hi_mutex);
    for (i=0; i<HG_TSIZE; ++i) {
		struct hg	*phg = &hgtable[i];
		if (phg->hg_state != HGS_IDLE || phg->hg_ifnum != ifnum)
			continue;
		phg->hg_state = HGS_DELAYING;
		panic("igmp_settimers not be implemented fully\n");
		//tmset(HostGroup.hi_uport, HG_TSIZE, phg, hgrand());
	}
	unlock(&HostGroup.hi_mutex);
	return OK;
}


