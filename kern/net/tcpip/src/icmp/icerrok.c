#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  icerrok -  is it ok to send an error response?
 *------------------------------------------------------------------------
 */
int32_t icerrok(struct ep *pep)
{
	struct ip *pip = (struct ip*)pep->ep_data;
	struct icmp *pic = (struct icmp*) pip->ip_data;

	/* don't send errors about error packets... */
	if (pip->ip_proto == IPT_ICMP) {
		switch(pic->ic_type) {
			case ICT_DESTUR:
			case ICT_REDIRECT:
			case ICT_SRCQ:
			case ICT_TIMEX:
			case ICT_PARAMP:
			     return false;
			default:
			     break;
		}	
	}
	/* ...or other than the first of a fragment */
	if (pip->ip_fragoff & IP_FRAGOFF) {
		return false;
	}
	/* ...or broadcast packets */

	if (isbrc(pip->ip_dst)) {
		return false;
	}
	return true;
}

