#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  cksum  -  Return 16-bit ones complement of 16-bit ones complement sum 
 *------------------------------------------------------------------------
 */
short cksum(unsigned short* buf, int nwords)
{
	unsigned long	sum;

	for (sum=0; nwords>0; nwords--)
		sum +=  (*buf++);
	sum = (sum >> 16) + (sum & 0xffff);	/* add in carry   */
	sum += (sum >> 16);			/* maybe one more */
	return ~sum;
}


