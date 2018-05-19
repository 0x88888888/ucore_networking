#include <tcpip/h/network.h>

int		upalloc(void);
unsigned short	udpnxtp(void);

/*------------------------------------------------------------------------
 *  dgmopen  -  open a fresh datagram pseudo device and return descriptor
 *------------------------------------------------------------------------
 */
int 
dgmopen(struct device *pdev, void* fspec0, void* lport0) {
	char  *fspec = (char*)fspec0;
	unsigned int lport = (unsigned int)lport0;
	struct dgblk  *pdg;
	struct upq *pup;
	int slot, upi, i;
    
    //dgtab中分的一个 dgblk
	if ((slot = dgalloc())== SYSERR) {
        return SYSERR;
	}
    //slot== dgtab的索引

    pdg = &dgtab[slot];
    lock(&udpmutex);
    if (lport == ANYLPORT) {
    	lport = udpnxtp(); //
    } else {
        for (i =0; i<UPPS; i++) {
            pup = &upqs[i];
            if (pup->up_valid && pup->up_port == lport) {
            	pdg->dg_state = DGS_FREE;
            	unlock(&udpmutex);
            	return SYSERR;
            }
        }
    }
    cprintf("lport : %d\n", lport);
    unlock(&udpmutex);
    if(dnparse(fspec, &pdg->dg_fip, &pdg->dg_fport)== SYSERR) {
    	pdg->dg_state = DGS_FREE;
    	return SYSERR;
    }
    cprintf("pdg->dg_fip=%x, pdg->dg_fport=%d\n",pdg->dg_fip, pdg->dg_fport);
    upi = upalloc();
    if (upi == SYSERR) {
    	pdg->dg_state = DGS_FREE;
    	return SYSERR;
    }
    pup = &upqs[upi];
    pup->up_port = pdg->dg_lport = lport;
    pdg->dg_xport = pup->up_xport;
    pdg->dg_upq = upi;
    pdg->dg_mode = DG_NMODE | DG_CMODE;
    return pdg->dg_dnum;//devtab中的索引
}

