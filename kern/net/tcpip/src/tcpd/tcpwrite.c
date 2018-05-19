#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpwrite  -  write one buffer from a TCP pseudo-device
 *------------------------------------------------------------------------
 */
int
tcpwrite(struct device *pdev, unsigned char *pch, unsigned int len) {
	return tcpwr(pdev, pch, len, TWF_NORMAL);
}
