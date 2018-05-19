#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcplisten -  do LISTEN state processing
 *------------------------------------------------------------------------
 *
 * LISTEN状态的tcb
 */
int
tcplisten(struct tcb *ptcb/*父tcb*/, struct ep *pep) {
    // 服务端listen状态的tcb，专门产生一个负责和对端通信的tcb
    // LISTEN 只接受syn 标记，也不接受实际数据
	struct tcb *newptcb/*子tcb*/, *tcballoc();
	struct ip  *pip = (struct ip*)pep->ep_data;
	struct tcp *ptcp=(struct tcp*)pip->ip_data;

	if (ptcp->tcp_code & TCPF_RST){
		//跳过rst标记，什么都没发生
		return OK;   /* "parent" TCB still in LISTEN */
	}
	if ((ptcp->tcp_code & TCPF_ACK) ||
		(ptcp->tcp_code & TCPF_SYN) ==0) {
		//tcpreset马上发送 rst
		return tcpreset(pep);
	}
	//分配tcb
	newptcb = tcballoc();
	//此时 newtcp->tcb_state == TCPS_CLOSED
	//开始初始化 newptcb,设置newptcb->tcb_code = TCPF_SYN;
	if ((int)newptcb == SYSERR || tcpsync(newptcb) == SYSERR) {
		return SYSERR;
	}
	//子tcb进入TCPS_SYNRCVD状态，父tcb依旧在LISTEN状态
	newptcb->tcb_state = TCPS_SYNRCVD;
	newptcb->tcb_ostate = TCPO_IDLE;
	newptcb->tcb_error = 0;
	newptcb->tcb_pptcb = ptcb;    /* for ACCEPT	*/

	newptcb->tcb_rip = pip->ip_src;
	newptcb->tcb_rport = ptcp->tcp_sport;
	newptcb->tcb_lip = pip->ip_dst;
	newptcb->tcb_lport = ptcp->tcp_dport;
    
    cprintf("listen newptcb->tcp_rip = %x\n",newptcb->tcb_rip);
    cprintf("listen newptcb->tcb_lip = %x\n",newptcb->tcb_lip);
    

    //用ptcb的信息去初始化newptcb的窗口信息
	tcpwinit(ptcb, newptcb, pep); /* initialize window data	*/
    
	newptcb->tcb_finseq = newptcb->tcb_pushseq = 0;

	newptcb->tcb_flags = TCBF_NEEDOUT;
	TcpPassiveOpens++;
	//处理过来的syn标记，发送ack和syn
	tcpdata(newptcb, pep);
	unlock(&(newptcb->tcb_mutex));
	return OK;
}

