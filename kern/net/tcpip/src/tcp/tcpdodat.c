#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpdodat -  do input data processing
 *------------------------------------------------------------------------
 * 
 * 处理收到的TCPF_FIN,TCPF_URG,TCPF_PSH标记
 *  tcpestablished
 *  tcpfin1
 *  tcpfin2
 *  tcplisten
 *  tcpsynrcvd
 *  tcpsynsent
 *  tcptimewait
 *     tcpdata
 *        tcpdodat
 *
 *   first = min(ptcp->tcp_seq, ptcb->tcb_rnext)
 */
int
tcpdodat(struct tcb *ptcb, struct tcp *ptcp, tcpseq first, unsigned int datalen) {
	int wakeup = 0;

	if (ptcb->tcb_rnext == first) {
		//接收缓冲区中没有窟窿
		if (datalen > 0) {
			//合并接收缓冲区中的数据
			tfcoalesce(ptcb, datalen, ptcp);
			ptcb->tcb_flags |= TCBF_NEEDOUT;
			wakeup++;
		}
		if (ptcp->tcp_code & TCPF_FIN) {
			/*
			 * 接收打动对端的FIN标记，表示对端不会再发送数据了
			 * 所以标记不再接收数据
			 * 包含FIN标记的数据包可能比普通数据先到,
			 
			 */
			ptcb->tcb_flags |= TCBF_RDONE | TCBF_NEEDOUT;
			ptcb->tcb_rnext++;
			wakeup++;
		}
		if (ptcp->tcp_code & (TCPF_PSH | TCPF_URG)) {
			ptcb->tcb_flags |= TCBF_PUSH;
			wakeup++;
		}
		if (wakeup){
			//有数据可以读取了
			tcpwakeup(READERS, ptcb);
		}
	} else {
		/*
		 * 接收缓冲区中有窟窿,
		 * 将数据插入ptcb->tcb_rsegq队列
		 *
		 */
		/* process delayed controls */
		if (ptcp->tcp_code & TCPF_FIN) {
			//记录fin的最后序号
			ptcb->tcb_finseq = ptcp->tcp_seq + datalen;
		}

		if (ptcp->tcp_code & (TCPF_PSH | TCPF_URG)){
			//记录push时的最后一个字节序号
			ptcb->tcb_pushseq = ptcp->tcp_seq + datalen;
		}
        
        // 因为有窟窿，数据没有全部到达，
        // 所以去掉 FIN, PSH标记
		ptcp->tcp_code &= ~(TCPF_FIN | TCPF_PSH);
		tfinsert(ptcb, first, datalen);
	}
	return OK;
} 


