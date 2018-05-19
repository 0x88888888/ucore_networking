#include <tcpip/h/network.h>

#define UDP_ALEN    IP_ALEN       /* length of src+dst, in shorts  */

/*------------------------------------------------------------------------
 *  udpcksum -  compute a UDP pseudo-header checksum
 *------------------------------------------------------------------------
 */
unsigned short
udpcksum(struct ep *pep, int len)
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	udp	*pudp = (struct udp *)pip->ip_data;
	unsigned	short	*sptr;
	unsigned	long ucksum;
	int		i;

	ucksum = 0;

	sptr = (unsigned short *) &pip->ip_src;
	/* 2*IP_ALEN octets = IP_ALEN shorts... */
	/* they are in net order.		*/
	for (i=0; i<IP_ALEN; ++i)
		ucksum += *sptr++;
	sptr = (unsigned short *)pudp;
	ucksum += htons(IPT_UDP + len);
	if (len % 2) {
		((char *)pudp)[len] = 0;	/* pad */
		len += 1;	/* for the following division */
	}
	len >>= 1;	/* convert to length in shorts */

	for (i=0; i<len; ++i)
		ucksum += *sptr++;
	ucksum = (ucksum >> 16) + (ucksum & 0xffff);
	ucksum += (ucksum >> 16);

	return (short)(~ucksum & 0xffff);
}




