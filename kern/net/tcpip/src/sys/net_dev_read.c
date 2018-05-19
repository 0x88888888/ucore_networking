#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  read  -  read one or more bytes from a device
 *------------------------------------------------------------------------
 */
int
net_dev_read(int descrp, void *buff, unsigned int len) {
    struct device*  pdev;
    if (isbaddev(descrp)) {
       return SYSERR;
    }
    pdev = &devtab[descrp];
    return ((pdev->d_read)(pdev,buff, len));	
}

