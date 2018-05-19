#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpdata -  process an input segment's data section
 *
 *  处理输入进来的数据
 *  
 *  tcpestablished
 *  tcpfin1
 *  tcpfin2
 *  tcplisten
 *  tcpsynrcvd
 *  tcpsynsent
 *  tcptimewait
 *        tcpdata
 *------------------------------------------------------------------------
 *
 * pep是接收到的数据
 */
int
tcpdata(struct tcb *ptcb, struct ep *pep) {
    struct ip   *pip = (struct ip*) pep->ep_data;
    struct tcp  *ptcp= (struct tcp*) pip->ip_data;
    tcpseq   first, last, wlast;
    unsigned int  pb;
    int    datalen, rwindow, i, pp;
    
    //处理 URG标记
    if (ptcp->tcp_code & TCPF_URG) {
    	//urgent point数据所在位置
    	int rup = ptcp->tcp_seq + ptcp->tcp_urgptr;
#ifdef  BSDURG
    	    rup--;
#endif    	
    	if (!(ptcb->tcb_flags & TCBF_RUPOK) ||
    		SEQCMP(rup, ptcb->tcb_rupseq) > 0) {
    		//记录urgent data位置，
            //TCBF_RUPOK表明接收到urgent数据了
    		ptcb->tcb_rupseq = rup;
    	    ptcb->tcb_flags  |= TCBF_RUPOK;
    	}
    }
    
    //处理 SYN标记请求
    if(ptcp->tcp_code & TCPF_SYN) {
    	ptcb->tcb_rnext++; //希望得到的下一个 sequence加 1
    	ptcb->tcb_flags |= TCBF_NEEDOUT;
        //跳过syn标记占用的这个seq,seq就是标记数据的seq了
    	++ptcp->tcp_seq;   /* so we start with data */
    }
    
    //数据长度
    datalen = pip->ip_len - IP_HLEN(pip) - TCP_HLEN(ptcp);
    //接收缓冲区中，已经接收的数据大小
    rwindow = ptcb->tcb_rbsize - ptcb->tcb_rbcount;
    //本端接收窗口的最后一个位置 sequence
    wlast = ptcb->tcb_rnext + rwindow -1;

    first = ptcp->tcp_seq;
    //本次对端发过来的数据的最后一个 sequence
    last = first + datalen -1;

    if(SEQCMP(ptcb->tcb_rnext, first) > 0) {
    	//first的位置比期望的ptcb->rnext小,这个可能是对方重复发送？
    	datalen -= ptcb->tcb_rnext  - first;
    	first = ptcb->tcb_rnext;    //跳过一部分数据
    }

    if(SEQCMP(last, wlast) > 0) {
    	//实际到达数据最后一个序号last大于窗口可接受的最后序号wlast了
        //需要丢弃超过接收能力的那些数据 
    	datalen -= last - wlast;
        /*
         * 数据不完整，
         * 如果对方在这个数据包带了FIN标记，抛弃这个FIN标记,
         * 让对方重新发送本次不能接收的数据
         */
    	ptcp->tcp_code &= ~TCPF_FIN; /* cutting it off */
    }
    
    // 空闲接受缓冲区的末尾
    pb =  ptcb->tcb_rbstart + ptcb->tcb_rbcount; // ==rnxt , in buf
          //(本次对端发过来的数据的首个sequence - 希望接受的 sequence ,有可能是0 或者大于0)
    pb += first - ptcb->tcb_rnext;    // 接收缓冲区中可能有窟窿，先接受后面的的sequence数据，前面的没有到达

    pb %= ptcb->tcb_rbsize;           // 环形缓冲去处理

    pp = first - ptcp->tcp_seq;       //

    for(i=0; i<datalen; ++i) {
        //复制对端发送过来的数据
    	ptcb->tcb_rcvbuf[pb] = ptcp->tcp_data[pp++];
    	if(++pb >= ptcb->tcb_rbsize){
            //环
    		pb = 0;
    	}
    }
    //处理收到的TCPF_FIN,TCPF_URG,TCPF_PSH标记
    tcpdodat(ptcb, ptcp, first, datalen);   // deal with it
    
    if (ptcb->tcb_flags & TCBF_NEEDOUT) {
    	tcpkick(ptcb);
    }
    return OK;
}

