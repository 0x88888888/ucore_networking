#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpnet2h -  convert TCP header fields from net to host byte order
 *------------------------------------------------------------------------
 */
struct tcp *
tcpnet2h(struct tcp *ptcp)
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


