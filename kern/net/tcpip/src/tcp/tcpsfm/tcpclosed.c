#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpclosed 
 *------------------------------------------------------------------------
 *
 * CLOSED状态表示tcb已经被分配，但是还没有开始使用
 *
 * tcpinp
 *    tcpclosed
 */
int
tcpclosed(struct tcb *ptcb, struct ep *pep) {
	/* CLOSED状态不不接受任何数据，发送reset标记，
	 * 但是本端的这个tcb没有被回收
	 * tcpreset 马上发送 rst
	 */
	tcpreset(pep);
	return SYSERR;
}
