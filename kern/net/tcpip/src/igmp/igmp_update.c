#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  igmp_update  -  send (delayed) IGMP host group updates
 *------------------------------------------------------------------------
 */
int32_t igmp_update(void) {
	struct hg   *phg;
	while(true) {
		cprintf("igmp_update not be implemented\n");
		lock(&HostGroup.hi_mutex);
        if (phg->hg_state == HGS_DELAYING) {
        	phg->hg_state = HGS_IDLE;
        	igmp(IGT_HREPORT, phg->hg_ifnum, phg->hg_ipa);
        }
		unlock(&HostGroup.hi_mutex);
	}
	return OK;
}
