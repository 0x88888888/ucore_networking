#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpok -  检查接受到的tcp报文的有效性 ，判断该报文的数据是否在接受窗口之内
 *   
 *------------------------------------------------------------------------
 *
 * tcpinp
 *    tcpok
 */
bool tcpok(struct tcb *ptcb, struct ep *pep) {
	struct  ip   *pip  = (struct ip *)pep->ep_data;
	struct  tcp  *ptcp = (struct tcp *) pip->ip_data;
	int     seglen, rwindow;
	tcpseq  wlast, slast;
	bool    rv;
    
    // 在CLOSED,LISTEN,SYN_SENT状态中，不会拒绝任何报文的
	if (ptcb->tcb_state < TCPS_SYNRCVD) {
		return true;
	}
    
    //tcp数据长度
	seglen = pip->ip_len - IP_HLEN(pip) - TCP_HLEN(ptcp);
	/* add SYN and FIN */
	//SYN和FIN报文要占据一个位置
	if (ptcp->tcp_code & TCPF_SYN) {
		++seglen;
	}
	if (ptcp->tcp_code & TCPF_FIN) {
		++seglen;
	}
    
    //接收窗口大小
	rwindow = ptcb->tcb_rbsize - ptcb->tcb_rbcount;
	
	if (rwindow == 0 && seglen == 0) {
		//没有空间并且对方发过来数据为空
		//FIN,PSH,RST,URG
		return ptcp->tcp_seq == ptcb->tcb_rnext;
	}

    //窗口内可接受的最大序号
	wlast = ptcb->tcb_rnext + rwindow - 1;
         /* ptcp->tcp_seq 落在 tcb_rnext 到 wlast内 */
	rv = (ptcp->tcp_seq - ptcb->tcb_rnext) >= 0 && (ptcp->tcp_seq - wlast) <= 0;
	if (seglen == 0) {
		//ptcp没有数据
        return rv;
	}

	//到这里说明ptcp是有数据的
	// slast为当前对端传过来的tcp数据的末尾序号
	slast = ptcp->tcp_seq + seglen - 1;
	// slast 落在 tcb_rnext 到 wlast内
	/*
	 * 用异或是因为有可能 ptcp->tcp_seq < ptcb->tcb_rnext < slast < wlast
	 * 也就是部分数据已经被接收，部分没有被接收，需要接收后面的部分
	 *
	 */
	rv |= (slast - ptcb->tcb_rnext) >= 0 && (slast - wlast) <= 0;
    

    if (rwindow == 0) {
    	//接受窗口没有了，那就直接发送过来的数据也丢弃掉算了，没地方放了
    	pip->ip_len = IP_HLEN(pip) + TCP_HLEN(ptcp);
    }
    
    return rv;
}

