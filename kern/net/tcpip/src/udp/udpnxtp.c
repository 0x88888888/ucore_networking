#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  udpnxtp  -  return the next available UDP local "port" number
 *  调用这个函数的时候，必须已经获得udpmutex
 *  返回一个 端口
 *------------------------------------------------------------------------
 */
unsigned short udpnxtp() {
	static unsigned short lastport = ULPORT;
	bool        inuse = true;
	struct      upq *pup;
	int i;

	while (inuse) {
		lastport++; //下一个端口号
		if (lastport == 0) {
			//循环
			lastport = ULPORT;
		}
		inuse =false;
		for (i=0;!inuse && i<UPPS; i++) {
			pup = &upqs[i];
			inuse = pup->up_valid && pup->up_port == lastport;
		}
	}
	return lastport;
}

