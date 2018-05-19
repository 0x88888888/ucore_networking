#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpkick -  make sure we send a packet soon
 *------------------------------------------------------------------------
 *
 * 给输出进程发送事件，输出数据
 *
 * tcpswindow
 * tcpdata
 * tcpostate
 * tcpclose
 * tcpcon
 * tcpgetdata
 * tcpwr
 *    tcpkick
 */
int
tcpkick(struct tcb *ptcb) {
	int tcbnum = ptcb - &tcbtab[0];
	void *tv;
	tv = MKEVENT(SEND, tcbnum);
	if ( (ptcb->tcb_flags & TCBF_DELACK) && !tmleft(tcps_oport, tv)){
		//有延时ack
		tmset(tcps_oport, TCPQLEN, tv, TCP_ACKDELAY);
	} else if (pcount(tcps_oport) <  TCPQLEN) {
		//通知tcpout进程，发送数据
        psend(tcps_oport, (int)tv); // send now;
	}
	return OK;
}

