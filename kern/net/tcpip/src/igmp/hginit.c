#include <tcpip/h/network.h>

extern	int	hgseed;
struct	hginfo	HostGroup;
struct	hg	hgtable[HG_TSIZE];

/*------------------------------------------------------------------------
 *  hginit  -  initialize the host group table
 *------------------------------------------------------------------------
 */
void hginit(void) {
	int i;
	mutex_init(&HostGroup.hi_mutex);
	HostGroup.hi_valid = true;
	kernel_thread(igmp_update, NULL, 0);
	for( i =0; i< HG_TSIZE ;i++) {
		hgtable[i].hg_state = HGS_FREE;
	}
    hgseed = nif[NI_PRIMARY].ni_ip;
	//memcpy(&hgseed ,nif[NI_PRIMARY].ni_ip, IP_ALEN);
	unlock(&HostGroup.hi_mutex);
	
	rtadd(ig_allhosts, ig_allDmask, ig_allhosts, 0, NI_PRIMARY,RT_INF);
    hgjoin(NI_PRIMARY, ig_allhosts, true);
}

