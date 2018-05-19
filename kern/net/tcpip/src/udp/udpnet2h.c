#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  udpnet2h -  convert UDP header fields from net to host byte order
 *------------------------------------------------------------------------
 */
void udpnet2h(struct udp* pudp)
{
	pudp->u_src = ntohs(pudp->u_src);
	pudp->u_dst = ntohs(pudp->u_dst);
	pudp->u_len = ntohs(pudp->u_len);
}
