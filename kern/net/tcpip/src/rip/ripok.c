#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ripok  -  determine if a received RIP route is ok to install
 *------------------------------------------------------------------------
 */
int ripok(struct riprt *rp) {
    unsigned char  net;

	if (rp->rr_family != AF_INET)
		return false;
	if (rp->rr_metric > RIP_INFINITY)
		return false;
	if (IP_CLASSD(rp->rr_ipa) || IP_CLASSE(rp->rr_ipa))
		return false;
	net = (ntohl(rp->rr_ipa) & 0xff000000) >> 24;
	if (net == 0 && rp->rr_ipa != ip_anyaddr)
		return false;		/* net 0, host non-0		*/
	if (net == 127)
		return false;		/* loopback net			*/
	return true;
}

