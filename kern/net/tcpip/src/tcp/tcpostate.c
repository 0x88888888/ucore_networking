#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpostate - do TCP output state processing after an ACK
 *------------------------------------------------------------------------
 * 在接收到一个ack后，修改输出从状态
 *
 * tcpacked
 *   tcpostate
 */
int
tcpostate(struct tcb *ptcb, int acked) {
	if (acked <= 0) {
		return OK;   /* no state change */
	}

	if (ptcb->tcb_ostate == TCPO_REXMT) {
		//表明重新发送成功了
        ptcb->tcb_rexmtcount = 0;
        ptcb->tcb_ostate = TCPO_XMIT;
	}
	if (ptcb->tcb_sbcount == 0) {
		//没有数据需要发送了
		ptcb->tcb_ostate = TCPO_IDLE;
		return OK;
	}
	//提醒 tcpout进程，发送数据
    tcpkick(ptcb);
    return OK;
}
