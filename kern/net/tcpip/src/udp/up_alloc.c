#include <tcpip/h/network.h>


struct	upq	upqs[UPPS];

int upalloc(){
	struct upq *pup;
	int    i;

	lock(&udpmutex);
	for (i=0; i<UPPS; i++) {
		pup = &upqs[i];
		if (!pup->up_valid) {
			pup->up_valid = true;
			pup->up_port = 0;
			pup->up_pid = BADPID;
			                // 50
			pup->up_xport = pcreate(UPPLEN);
			unlock(&udpmutex);
			return i;
		}
	}
    unlock(&udpmutex);
	return SYSERR;
}