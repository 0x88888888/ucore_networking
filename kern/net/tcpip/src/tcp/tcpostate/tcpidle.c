#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpidle - handle events while a connection is idle
 *------------------------------------------------------------------------
 * 
 * tcpout进程 idle状态
 *  
 */
int
tcpidle(int tcbnum, int event) {

	if (event == SEND) { // newtcb 回复 syn,ack的时候用到
		tcpxmit(tcbnum, event);
	}
	
	return OK;
}

