#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * ripmetric - compute the RIP metric for a route we advertise
 *------------------------------------------------------------------------
 */
int ripmetric(struct route *prt, int ifnum) {
	/* only advertise the net route for our interfaces */
	if (prt->rt_ifnum == NI_LOCAL &&
		(prt->rt_mask == ip_maskall)){
		return SYSERR;
	}

	if(prt->rt_ifnum == ifnum) {
		return RIP_INFINITY;	/* poison reverse */
	}
	/* else, add one to our cost */
	return prt->rt_metric + 1;
}

