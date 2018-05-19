#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpkilltimers -  kill all outstanding timers for a TCB
 *------------------------------------------------------------------------
 *
 * 清空所有ptcb的 tcpout中的事件(SEND,RETRANSMIT,PERSIST)
 *
 * tcpabort
 * tcpsynsent
 * tcpwait
 * tcbdealloc
 *    tcpkilltimers
 *
 */
int
tcpkilltimers(struct tcb *ptcb) {
	int tcbnum = ptcb - &tcbtab[0];

	tmclear(tcps_oport, MKEVENT(SEND, tcbnum));
	tmclear(tcps_oport, MKEVENT(RETRANSMIT, tcbnum));
	tmclear(tcps_oport, MKEVENT(PERSIST, tcbnum));
	return OK;
}




