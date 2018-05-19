#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcppersist - handle events while the send window is closed
 *
 *
 * tcpout进程的 PERSIST状态
 *------------------------------------------------------------------------
 *
 */
int tcppersist(int tcbnum, int event) {
	struct tcb   *ptcb = &tcbtab[tcbnum];

	if (event != PERSIST && event != SEND) {
		return OK;  /* ignore everything else */
	}
	tcpsend(tcbnum, TSF_REXMT);
	ptcb->tcb_persist = min(ptcb->tcb_persist<<1, TCP_MAXPRS);
	//设置persist事件
	tmset(tcps_oport, TCPQLEN, MKEVENT(PERSIST, tcbnum),ptcb->tcb_persist);
	return OK;
}


