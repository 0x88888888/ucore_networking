#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * ipfinit  -  initialize IP fragment queue data structures
 *------------------------------------------------------------------------
 */
void ipfinit()
{
	int i;
	mutex_init(&ipfmutex);
	for (i=0; i<IP_FQSIZE; ++i) {
		ipfqt[i].ipf_state = IPFF_FREE;
	}
}



