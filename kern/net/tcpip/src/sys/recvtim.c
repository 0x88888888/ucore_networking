#include <tcpip/h/network.h>

int32_t
recvtim(int maxwait) {
	//int intr_flag;
	int timeout;
	timeout=do_sleep(maxwait);
	if(timeout>0) return TIMEOUT;
	return 0;
}


