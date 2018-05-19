#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpputc  -  
 *------------------------------------------------------------------------
 */
int
tcpputc(struct device *pdev, unsigned char ch) {
	return tcpwr(pdev, &ch, 1, TWF_NORMAL);
}
