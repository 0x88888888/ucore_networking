#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dgclose  -  close a datagram pseudo device, making it available again
 *------------------------------------------------------------------------
 */
int
dgclose(struct device *pdev) {
	struct  dgblk*  pdg;
	struct  upq*    pup;
    
	pdg = (struct dgblk *)pdev->dvioblk;
	pup = &upqs[pdg->dg_upq];
	lock(&udpmutex);
	pup->up_port = 0;
	if (pup->up_xport >=0) {
       pdelete(pup->up_xport, NULL);
	}
	pup->up_valid =false;
    unlock(&udpmutex);
    pdg->dg_state = DGS_FREE;
    return OK;
}

