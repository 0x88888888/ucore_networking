#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpmcntl - control function for the master TCP pseudo-device
 *------------------------------------------------------------------------
 */
int
tcpmcntl(struct device *pdev, int func, void *arg1) {
	int rv;
	if ( pdev != &devtab[TCP]) {
		return SYSERR;
	}
    
    switch(func) {
    	case TCPC_LISTENQ:
    	     tcps_lqsize =(int)arg1;
    	     rv = OK;
    	     break;
    	default:
    	     rv = SYSERR;
    }
    return rv;
}
