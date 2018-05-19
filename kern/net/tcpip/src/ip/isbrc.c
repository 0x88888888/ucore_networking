#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  isbrc  -  Is "dest" a broadcast address?
 *
 *  是否是广播用的IP地址
 *------------------------------------------------------------------------
 */
int isbrc(IPaddr dest) {
	int inum;

	/* all 0's and all 1's are broadcast */
    // 0.0.0.0, 
    // 255.255.255.255
	if ( (dest == ip_anyaddr) ||
		(dest == ip_maskall) ) {
		return true;
	}

	/* check real broadcast address and BSD-style for net & subnet 	*/
	for (inum=0; inum<Net.nif; ++inum) {
		if ((dest == nif[inum].ni_brc) ||
		    (dest == nif[inum].ni_nbrc) ||
		    (dest == nif[inum].ni_subnet) ||
		    (dest == nif[inum].ni_net))
		return true;
	}
	return false;
}

