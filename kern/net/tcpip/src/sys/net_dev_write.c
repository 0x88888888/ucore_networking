#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  write  -  write 1 or more bytes to a device
 *------------------------------------------------------------------------
 */
int
net_dev_write(int descrp, void *buff, unsigned len) {
    struct device*  pdev;
    if (isbaddev(descrp)) {
	   return SYSERR;
    }
    pdev = &devtab[descrp];
    return ((pdev->d_write)(pdev, buff, len));
}



