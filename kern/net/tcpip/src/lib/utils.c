#include <tcpip/h/network.h>


/*------------------------------------------------------------------------
 * dot2ip  -  Convert a string of dotted decimal to an unsigned integer
 *            return host byte order ip value
 *------------------------------------------------------------------------
 */
uint32_t	dot2ip (
	  char	 *dotted,		/* IP address in dotted decimal	*/
	  IPaddr *result		/* Location to which binary IP	*/
					/*    address will be stored	*/
					/*    (host byte order)		*/
	)
{
	int32_t	seg;			/* Counts segments		*/
	int32_t	nch;			/* Counts chars within segment	*/
	char	ch;			/* Next character		*/
	uint32_t	ipaddr;			/* IP address in binary		*/
	uint32_t	val;			/* Binary value of one segment	*/

	/* Input must have the form  X.X.X.X, where X is 1 to 3 digits	*/

	ipaddr = 0;
	for (seg=0 ; seg<4 ; seg++) {    /* For each segment		*/
	    val = 0;
	    
	    for (nch=0 ; nch<4; nch++) { /* Up to four chars per segment*/
		    ch = *dotted++;
			if ( (ch==NULLCH) || (ch == '.') ) {
				if (nch == 0) {
					return SYSERR;
				} else {
					break;
				}
			}

			/* Too many digits or non-digit is an error */

			if ( (nch>=3) || (ch<'0') || (ch>'9') ) {
				if (ch == ':' && seg == 3) {
                    ipaddr = (ipaddr << 8) | val;           
					goto right;
				}
				return SYSERR;
			}
			val = 10*val + (ch-'0');
	    }

	    if (val > 255) {	/* Out of valid range */
		    return SYSERR;
	    }
	    ipaddr = (ipaddr << 8) | val;


	    if (ch == NULLCH) {
		    break;
	    }
	}
	
	if ( (seg >= 4) || (ch != NULLCH) ) {
		
		return SYSERR;
	}

right:	
	*result = ipaddr;
	return OK;
}








