#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpabort -  abort an active TCP connection
 *              干掉所有ptcb上的事件，终止一个tcp连接
 *------------------------------------------------------------------------
 *
 *  tcprexmt
 *  tcpclosewait
 *  tcpestablished
 *  tcpfin1
 *  tcpfin2
 *  tcplastack
 *  tcpsynrcvd
 *       tcpabort
 *
 */
int
tcpabort(struct tcb *ptcb, int error) {

	//干掉ptcb->tcps_oport上的SEND,RETRANSMIT,PERSIST事件
	tcpkilltimers(ptcb);
	//不再接收和发送数据
	ptcb->tcb_flags |= TCBF_RDONE|TCBF_SDONE;
	ptcb->tcb_error = error;
	//唤醒等待上ptcb->tcb_rsema 和 ptcb->tcb_ssema的进程
	tcpwakeup(READERS|WRITERS, ptcb);
	return OK;
}
