#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  hgarpadd  -  add an ARP table entry for a multicast address
 *------------------------------------------------------------------------
 */
int hgarpadd(unsigned int ifnum, IPaddr ipa) {
	struct  netif    *pni = &nif[ifnum];
	struct  arpentry *pae , *arpalloc();
	int     ifdev= nif[ifnum].ni_dev; 
	int     intr_flag;

	local_intr_save(intr_flag);
	pae = arpalloc();
	if (pae == NULL) {
        local_intr_restore(intr_flag);
        return SYSERR;
	}
	pae->ae_hwtype = pni->ni_hwtype;
	pae->ae_prtype = EPT_IP;
	pae->ae_pni = pni;
	pae->ae_hwlen = pni->ni_hwa.ha_len;
	pae->ae_prlen = IP_ALEN;
	if(pae->ae_queue ==NULL){
		pae->ae_queue = create_queue(20);
	}
	
	memcpy(pae->ae_pra, &ipa, IP_ALEN);
	if (pni->ni_mcast){
		//panic("hgarpadd not be implemented fully\n");
		(pni->ni_mcast)(NI_MADD, ifdev, pae->ae_hwa, ipa);
	} else {
		memcpy(pae->ae_hwa, pni->ni_hwb.ha_addr, pae->ae_hwlen);
	}
    pae->ae_ttl = ARP_INF;
    pae->ae_state = AS_RESOLVED;
    local_intr_restore(intr_flag);
    return OK;
}

