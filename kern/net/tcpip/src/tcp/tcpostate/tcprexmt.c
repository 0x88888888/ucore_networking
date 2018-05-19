#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcprexmt - handle TCP output events while we are retransmitting
 *------------------------------------------------------------------------
 *
 *   tcpout 
 *   tcpxmit
 *       tcprexmt
 *
 */
int
tcprexmt(int tcbnum, int event) {

	struct tcb  *ptcb = &tcbtab[tcbnum];

	if (event != RETRANSMIT) {
		return OK;  /* ignore others while retransmitting */
	}
	if (++ptcb->tcb_rexmtcount > TCP_MAXRETRIES) {
		tcpabort(ptcb, TCPE_TIMEDOUT);
		return OK;
	}
	//重新发送数据
	tcpsend(tcbnum, TSF_REXMT);
	
	//设置一个超时重发事件,重发的时间间隔会指数上升
	tmset(tcps_oport, TCPQLEN, MKEVENT(RETRANSMIT, tcbnum),
		min(ptcb->tcb_rexmt<<ptcb->tcb_rexmtcount, TCP_MAXRXT));

	if (ptcb->tcb_ostate != TCPO_REXMT) {
		//首次重发
		ptcb->tcb_ssthresh = ptcb->tcb_cwnd;  
	}
	//出现重发了，网络有可能堵塞，阀值成倍的降低
	ptcb->tcb_ssthresh = min(ptcb->tcb_swindow,ptcb->tcb_ssthresh)/2;

	if (ptcb->tcb_ssthresh < ptcb->tcb_smss) {
		ptcb->tcb_ssthresh = ptcb->tcb_smss;
	}
	ptcb->tcb_cwnd = ptcb->tcb_smss;
	return OK;
}


