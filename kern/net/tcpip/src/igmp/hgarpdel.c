#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  hgarpdel  -  remove an ARP table entry for a multicast address
 *------------------------------------------------------------------------
 */
int hgarpdel(unsigned ifnum, IPaddr ipa) {
	struct netif      *pni = &nif[ifnum];
	int    ifdev = nif[ifnum].ni_dev;
	struct arpentry *pae;
	int intr_flag=0;

	local_intr_save(intr_flag);
    if ((pae = arpfind((unsigned char*)&ipa, EPT_IP, pni))) {
    	pae->ae_state = AS_FREE;
    }
    if (pni->ni_mcast) {
    	(pni->ni_mcast)(NI_MDEL, ifdev, pae->ae_hwa, ipa);
    }
    local_intr_restore(intr_flag);

	return OK;
}

