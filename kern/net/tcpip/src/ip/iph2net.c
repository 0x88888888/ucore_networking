#include <tcpip/h/network.h>


/*------------------------------------------------------------------------
 *  iph2net - convert an IP packet header from host to net byte order
 *------------------------------------------------------------------------
 */
struct ip *iph2net(struct ip* pip)
{
	/* NOTE: does not include IP options	*/

	pip->ip_len = htons(pip->ip_len);
	pip->ip_id = htons(pip->ip_id);
	pip->ip_fragoff = htons(pip->ip_fragoff);

	//pip->ip_proto = htons(pip->ip_proto);
	pip->ip_src = htonl(pip->ip_src);
	pip->ip_dst = htonl(pip->ip_dst);
    
	return pip;
}
