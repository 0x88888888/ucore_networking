#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  rtfree  -  remove one reference to a route
 *------------------------------------------------------------------------
 */
int rtfree(struct route *prt)
{
	if (!Route.ri_valid)
		return SYSERR;
    lock(&Route.ri_mutex);
    RTFREE(prt);
    unlock(&Route.ri_mutex);
    return OK;
}
