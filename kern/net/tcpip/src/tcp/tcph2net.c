#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcph2net -  convert TCP header fields from host to net byte order
 *------------------------------------------------------------------------
 */
struct tcp *
tcph2net(struct tcp *ptcp)
{
	/* NOTE: does not include TCP options */

	ptcp->tcp_sport = ntohs(ptcp->tcp_sport);
	ptcp->tcp_dport = ntohs(ptcp->tcp_dport);
	ptcp->tcp_seq = ntohl(ptcp->tcp_seq);
	ptcp->tcp_ack = ntohl(ptcp->tcp_ack);
	ptcp->tcp_window = ntohs(ptcp->tcp_window);
	ptcp->tcp_urgptr = ntohs(ptcp->tcp_urgptr);
	return ptcp;
}


