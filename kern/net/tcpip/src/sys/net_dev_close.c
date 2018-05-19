#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  close  -  close a device
 *------------------------------------------------------------------------
 */
int32_t
net_dev_close(int descrp)
{
	struct device *devptr;

	if (isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
	return( (devptr->d_close)(devptr));
}
