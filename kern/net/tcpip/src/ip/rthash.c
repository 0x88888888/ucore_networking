#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  rthash  -  compute the hash for "net"
 *------------------------------------------------------------------------
 */
 int rthash(IPaddr net) {
    int	bc = IP_ALEN;	/* # bytes to count	*/
	unsigned int hv = 0;		/* hash value		*/

	if (IP_CLASSA(net)) bc = 1;
	else if (IP_CLASSB(net)) bc = 2;
	else if (IP_CLASSC(net)) bc = 3;
	else if (IP_CLASSD(net))
		return ((net>>24) & 0xf0) % RT_TSIZE;
	while (bc--)
		hv += ((char *)&net)[bc] & 0xff;
	return hv % RT_TSIZE;
 }


