#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpacked - handle in-bound ACKs and do round trip estimates
 *  1.估算rtt值
 *  2.根据tcp_ack的值，处理掉 TCPF_SYN，TCPF_FIN，TCBF_SUPOK
 * 
 *------------------------------------------------------------------------
 *
 *  tcpclosewait
 *  tcpclosing
 *  tcpestablished
 *  tcpfin1
 *  tcpfin2
 *  tcplastack
 *  tcpsynrcvd
 *  tcpsynsent
 *  tcptimewait
 *        tcpacked
 *
 */
int
tcpacked(struct tcb *ptcb, struct ep *pep) {
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	tcp	*ptcp = (struct tcp *)pip->ip_data;
	int		acked;
	int     cacked; //记录SYN报文或者FIN报文占用序号的个数
	int     intr_flag;

	if (!(ptcp->tcp_code & TCPF_ACK)) {
		//没有ack标记，就不处理
		return SYSERR;
	}
    
    //被ack掉的数据量
	acked = ptcp->tcp_ack - ptcb->tcb_suna;

	cacked = 0;
	if (acked <= 0) {
		//已经ack过了，又来ack，重复
		return 0;  /* duplicate ACK */
	}
	
	if (SEQCMP(ptcp->tcp_ack, ptcb->tcb_snext) > 0) {
		//ack的序号超出了 将要 发送的序号，不对
		if (ptcb->tcb_state == TCPS_SYNRCVD) {
			//握手没有完成，reset掉，马上发送rst
			return tcpreset(pep);
		} else {
			//其余的，就发一个ack报文，不处理数据
			return tcpackit(ptcb, pep);
		}
	}
    
    //重新估算往返时间，并且更新拥塞窗口ptcb->tcb_cwnd
    //
	tcprtt(ptcb);
	//更新第一个发送了，但是未被确认的数据序号
	ptcb->tcb_suna = ptcp->tcp_ack;
	if (acked && (ptcb->tcb_code & TCPF_SYN)) {
		//收到响应syn标记的ack  , SYN_SENT状态会到这里
		//由于 SYN和syn + ack报文段需要占用一个序号，
	    //因此确认数据量ack减1，且同时 cacked加 1，清除ptcb->TCPF_SYN标记
	    //和TCBF_FIRSTSEND flag
		acked--;
		cacked++;
		ptcb->tcb_code &= ~TCPF_SYN;
		//不再是first send了
		ptcb->tcb_flags &= ~TCBF_FIRSTSEND;
	}
	
	if((ptcb->tcb_code & TCPF_FIN) && 
		SEQCMP(ptcp->tcp_ack, ptcb->tcb_snext) == 0 ) {
		//收到响应fin标记的ack 
		//如果收到报文段FIN标志，则由于FIN报文段占用一个序号，
		//因此acked减1，cacked加1
		acked--;
	    cacked++;
	    ptcb->tcb_code &=~TCPF_FIN;
	    ptcb->tcb_flags &= ~TCBF_SNDFIN;
	}

	if ((ptcb->tcb_flags & TCBF_SUPOK) &&
		SEQCMP(ptcp->tcp_ack, ptcb->tcb_supseq) >= 0) {
        //urgent 已经被对方处理
        ptcb->tcb_flags &= ~TCBF_SUPOK;
	}
    
	//由于已经确认，调整发送缓冲区信息
	ptcb->tcb_sbstart = (ptcb->tcb_sbstart+acked) % ptcb->tcb_sbsize;
    //未发送的字符减少，发送缓冲区空间增加了
    ptcb->tcb_sbcount -= acked;

    if (acked) {
    	//唤醒等待发送缓冲区的进程
    	local_intr_save(intr_flag);

    	//说明有进程在等待
    	signal(&(ptcb->tcb_ssema));
    	
    	local_intr_restore(intr_flag);
    }

    tcpostate(ptcb, acked + cacked);
    return acked;
}


