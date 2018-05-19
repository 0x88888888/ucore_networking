#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  open  -  open a connection to a device/file (parms 2 &3 are optional)
 *------------------------------------------------------------------------
 */
int 
net_dev_open(int descrp, const void *nam0, const void *mode0) {
	char* nam =(char *)nam0;
	char* mode =(char*)mode0;
	struct device*  pdev;
	
	int devtab_idx; /*d_open函数必须返回devtab中的索引*/

	if (isbaddev(descrp)) {
       return SYSERR;
	}
    pdev = &devtab[descrp];
    devtab_idx =((pdev->d_open)(pdev, (uint32_t)nam, (uint32_t)mode));
    return devtab_idx;
}

