#include <tcpip/h/network.h>

#define	TCPINCR		904

/*------------------------------------------------------------------------
 *  tcpiss -  set the ISS for a new connection 
 *            根据时间，得到一个 initial send sequence
 *------------------------------------------------------------------------
 */
 int
 tcpiss(void) {
 	static  int seq = 0;
 	//extern long clktime;   /* the system ticker	*/
    extern long gettime2();
 	if (seq == 0) {
 		seq = gettime2();
 	}
 	       // 904
 	seq += TCPINCR;
 	return seq;
 }

