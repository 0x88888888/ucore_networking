#include <tcpip/h/network.h>

int	modulus = 1009;		/* ~10 secs in 1/100'th secs	*/
int	offset = 523;		/* additive constant		*/
int	hgseed;			/* initialized in hginit()	*/

/*------------------------------------------------------------------------
 *  hgrand  -  return "random" delay between 0 & 10 secs (in 1/100 secs)
 *------------------------------------------------------------------------
 */
int hgrand(void)
{
	int rv;

	rv = ((modulus+1) * hgseed + offset) % modulus;
	if (rv < 0)
		rv += modulus;	/* return only positive values */
	hgseed = rv;
	return rv;
}

