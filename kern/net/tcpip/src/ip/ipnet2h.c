#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ipnet2h - convert an IP packet header from net to host byte order
 *------------------------------------------------------------------------
 */
struct ip* ipnet2h(struct ip *pip) {
	/* NOTE: does not include IP options	*/
	pip->ip_len = ntohs(pip->ip_len);
	pip->ip_id  = ntohs(pip->ip_id);
	pip->ip_fragoff = ntohs(pip->ip_fragoff);
    
	//pip->ip_proto = ntohs(pip->ip_proto);
	pip->ip_src = ntohl(pip->ip_src);
	pip->ip_dst = ntohl(pip->ip_dst);
	
	return pip;
}

