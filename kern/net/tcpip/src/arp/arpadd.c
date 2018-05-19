#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * arpadd - Add a RESOLVED entry to the ARP cache
 *------------------------------------------------------------------------
 */
struct arpentry* arpadd(struct netif *pni, struct arp* parp) {
	struct arpentry* pae;
	pae = arpalloc();

	pae->ae_hwtype = parp->ar_hwtype;
	pae->ae_prtype = parp->ar_prtype;
	pae->ae_hwlen = parp->ar_hwlen;
	pae->ae_prlen = parp->ar_prlen;
    pae->ae_pni = pni;
    if (pae->ae_queue == NULL) {
        pae->ae_queue = create_queue(20);
    }
    blkcopy(pae->ae_hwa, SHA(parp), parp->ar_hwlen);
	blkcopy(pae->ae_pra, SPA(parp), parp->ar_prlen);
	/*
	cprintf("arpadd : ae_hwa:%02x:%02x:%02x:%02x:%02x:%02x\n",
		pae->ae_hwa[0],
		pae->ae_hwa[1],
		pae->ae_hwa[2],
		pae->ae_hwa[3],
		pae->ae_hwa[4],
		pae->ae_hwa[5]);

	cprintf("arpadd : ae_pra:%02x:%02x:%02x:%02x\n",
		pae->ae_pra[0],
		pae->ae_pra[1],
		pae->ae_pra[2],
		pae->ae_pra[3]);
    */
	pae->ae_ttl = ARP_TIMEOUT ;
	pae->ae_state = AS_RESOLVED;
    return pae;
}



