#include <tcpip/h/network.h>


int tcpread(struct device *pdev, char *pch, unsigned len);

/*------------------------------------------------------------------------
 *  tcpgetc  -  read one character from a TCP pseudo-device
 *------------------------------------------------------------------------
 */
int
tcpgetc(struct device *pdev) {
	char    ch;
	int     cc;

	cc = tcpread(pdev ,&ch, 1);
	if (cc < 0) {
		return SYSERR;
	} else if (cc == 0) {
		return EOF;
	}
	/*else, valid data*/
	return ch;
}

