#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  udph2net -  convert UDP header fields from host to net byte order
 *------------------------------------------------------------------------
 */
void udph2net(struct udp* pudp)
{
	pudp->u_src = htons(pudp->u_src);
	pudp->u_dst = htons(pudp->u_dst);
	pudp->u_len = htons(pudp->u_len);
}


