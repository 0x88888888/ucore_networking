#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dgalloc  -  allocate a datagram psuedo device; return its descriptor
 *------------------------------------------------------------------------
 */
int dgalloc()
{
	struct dgblk   *pdg;
	int    i;
    
    mutex_init(&dgmutex);
	for (i = 0;i<Ndg; i++) {
        pdg = &dgtab[i];
        if (pdg->dg_state == DGS_FREE) {
        	pdg->dg_state = DGS_INUSE;
        	unlock(&dgmutex);
        	return i;
        }
	}
    unlock(&dgmutex);
    return SYSERR;
}

