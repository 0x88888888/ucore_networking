#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * arpfind - arptable中是否缓存相应接口、ip等匹配的表项
 *------------------------------------------------------------------------
 */
struct arpentry* arpfind(unsigned char *pra, int prtype, struct netif *pni) {
	struct arpentry  *pae;
	int              i;
	for (i=0; i<ARP_TSIZE; ++i) {
		pae = &arptable[i];
		if (pae->ae_state == AS_FREE){
			continue;
		}
        /*
        cprintf("pae->ae_pra %02x.%02x.%02x.%02x\n",
        	pae->ae_pra[0],
        	pae->ae_pra[1],
        	pae->ae_pra[2],
        	pae->ae_pra[3]);

        cprintf("pra %02x.%02x.%02x.%02x\n",
        	pra[0],
        	pra[1],
        	pra[2],
        	pra[3]);
        cprintf("pae->ae_pni == pni ? %d\n",pae->ae_pni == pni);
        cprintf("pae->ae_prtype == prtype? %d \n",pae->ae_prtype == prtype);
        cprintf("pae->ae_prtype =%d  prtype= %d  \n",pae->ae_prtype , prtype);
        */
		if (pae->ae_prtype == prtype &&
		    pae->ae_pni == pni &&
		    blkequ(pae->ae_pra, pra, pae->ae_prlen)){
			
			return pae;
	    }
	}
    return NULL;
}

