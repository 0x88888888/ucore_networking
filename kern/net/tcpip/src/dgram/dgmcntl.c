#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dgmcntl  -  control the network and datagram pseudo devices
 *------------------------------------------------------------------------
 */
int
dgmcntl(struct device* pdev, int com, void* arg1) {
	int status;
	switch (com)	{
	default:
       status = SYSERR;
    }
    return status;
}

