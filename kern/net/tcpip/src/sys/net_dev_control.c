#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  control  -  control a device (e.g., set the mode)
 *------------------------------------------------------------------------
 */
int32_t
net_dev_control(int descrp, int func, ...)
{
	va_list ap;
	struct  device *pdev;
	void    *arg1;
	void    *arg2;
	int     rv;
    
	va_start(ap, func);
	if (isbaddev(descrp) )
		return SYSERR;
	pdev = &devtab[descrp];
	arg1 = va_arg(ap, void *);
	arg2 = va_arg(ap, void *);

	rv = (pdev->d_ioctl)(pdev, func, arg1, arg2);
	va_end(ap);
	return rv;
}


