#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  init  --  initialize a device
 *------------------------------------------------------------------------
 */
int32_t init(int descrp) {
	struct device *devptr;
	if (isbaddev(descrp)) {
       return SYSERR;
	}
    devptr = &devtab[descrp];
    return ((devptr->d_init)(devptr));
}

