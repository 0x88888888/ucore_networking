#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpwait - (re)schedule a DELETE event for 2MSL from now
 *    在发送最后一个fin的ack之后，进入time_wait状态
 *------------------------------------------------------------------------
 *
 * tcpclosing
 *    tcpwait
 *
 * tcpfin1
 *    tcpwait
 *
 * tcpfin2
 *    tcpwait
 *
 * tcpsend
 *    tcpwait
 *
 * 最后一个fin的ack发送之后，会有2MSL的时间之后才会删除这个tcb
 * tcptimewait
 *       tcpwait
 */
int
tcpwait(struct tcb *ptcb) {
	int tcbnum = ptcb - &tcbtab[0];
    
    //tcpkilltimers删除任何与该tcb相关的正在进行的事件
	tcpkilltimers(ptcb);
	//2msl之后，在DELETE事件中删除ptcb
	tmset(tcps_oport, TCPQLEN, MKEVENT(DELETE, tcbnum), TCP_TWOMSL);
	return OK;
}