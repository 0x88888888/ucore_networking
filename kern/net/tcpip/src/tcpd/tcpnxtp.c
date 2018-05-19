#include <tcpip/h/network.h>

#define	IPPORT_RESERVED		1024	/* from BSD */

/*------------------------------------------------------------------------
 *  tcpnxtp  -  return the next available TCP local "port" number
 *------------------------------------------------------------------------
 */
unsigned short
tcpnxtp() {
	static unsigned short lastport = 1; /* #'s 1-1023 */
	unsigned short start;
	int i;

    lock(&tcps_tmutex);
    for (start = lastport++; start != lastport; ++lastport) {
    	if (lastport == IPPORT_RESERVED) {
    		lastport = 1;
    	}
    	for (i = 0; i< Ntcp; ++i) {
    		if (tcbtab[i].tcb_state != TCPS_FREE &&
    			tcbtab[i].tcb_lport == lastport) {
    			break;
    		}
    		if (i == Ntcp) {
    			break;
    		}
    	}
    }
    if (lastport == start) {
    	panic("out of TCP ports");
    }
    unlock(&tcps_tmutex);
    return lastport;
}


