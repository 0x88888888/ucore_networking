#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpsend -  根据tcb的设置和ptcb->tcb_sndbuf,发送tcp数据
 *------------------------------------------------------------------------
 *
 *  tcppersist
 *  tcprexmt
 *  tcpxmit
 *          tcpsend
 *
 *
 */
int
tcpsend(int tcbnum, bool rexmt) {
	struct tcb   *ptcb  =  &tcbtab[tcbnum];
	struct ep    *pep;
	struct ip    *pip;
	struct tcp   *ptcp;
	unsigned char *pch;
	unsigned int i, datalen,tocopy,off;
	int  newdata;

	pep = (struct ep*)kmalloc(sizeof(struct ep));
	if (pep == NULL) {
       return SYSERR;
	}
	pip = (struct ip*)pep->ep_data;
	pip->ip_src = ptcb->tcb_lip; //已经网络序
	pip->ip_dst = ptcb->tcb_rip; //已经网络序
    //在mss和swindow内可发送的数据量
	datalen = tcpsndlen(ptcb, rexmt, &off);  /* get length & offset */
    ptcp = (struct tcp*)pip->ip_data;
    ptcp->tcp_sport = ptcb->tcb_lport;
    ptcp->tcp_dport = ptcb->tcb_rport;

    if (!rexmt){
        //非重发
    	ptcp->tcp_seq = ptcb->tcb_snext;
    } else {
        //重发
    	ptcp->tcp_seq = ptcb->tcb_suna;
    }

    // 设置希望接收到的sequence
    ptcp->tcp_ack = ptcb->tcb_rnext;
    
    if ((ptcb->tcb_flags & TCBF_SNDFIN) &&
    	SEQCMP(ptcp->tcp_seq+datalen, ptcb->tcb_slast) == 0) {
        //在最后一段数据发送出去时，才标记fin
    	ptcb->tcb_code |= TCPF_FIN;
    }
    //设置 ptcp->tcp_code 
    ptcp->tcp_code = ptcb->tcb_code;

    
    ptcp->tcp_offset = TCPHOFFSET;

    if ((ptcb->tcb_flags & TCBF_FIRSTSEND) == 0) {
        //首次发送
    	ptcp->tcp_code |= TCPF_ACK;
    }

    if (ptcp->tcp_code & TCPF_SYN) {
        //只在syn时，通告max segment size
    	tcprmss(ptcb, pip);
    }

    if (datalen > 0) {
        //本实现特殊处理
    	ptcp->tcp_code |= TCPF_PSH;
    }
    
    //本端的接受窗口大小
    ptcp->tcp_window = tcprwindow(ptcb);
    if (ptcb->tcb_flags & TCBF_SUPOK) {
        //有urgent数据需要发送
        // up为ugent数据相对于 tcp_seq的位置
        short up = ptcb->tcb_supseq - ptcp->tcp_seq;

        if (up >=0) {
        	//有urgent数据
#ifdef  BSDURG
           ptcp->tcp_urgptr = up +1;  /* 1 past end */
#else   /* BSDURG */
           ptcp->tcp_urgptr = up;
#endif  /* BSDURG */
           ptcp->tcp_code |= TCPF_URG;       	
        } else {
        	ptcp->tcp_urgptr = 0;//没有urgent数据
        }
    }

    //tcp数据开始地址
    pch = &pip->ip_data[TCP_HLEN(ptcp)];
    i = (ptcb->tcb_sbstart+off) % ptcb->tcb_sbsize;
    for (tocopy=datalen; tocopy>0; --tocopy) {
    	*pch++ = ptcb->tcb_sndbuf[i];
    	if(++i >= ptcb->tcb_sbsize) {
    		i =0;
    	}
    }
    //
    ptcb->tcb_flags &= ~TCBF_NEEDOUT;   
    if(rexmt) {
    	newdata = ptcb->tcb_suna + datalen - ptcb->tcb_snext;
    	if (newdata < 0) {
    		newdata = 0;
    	}
    	TcpRetransSegs++;
    } else {
    	newdata = datalen;
        //syn,fin占据序号 ,
    	if (ptcb->tcb_code & TCPF_SYN) {
    		newdata++; /* SYN is part of the sequence */
    	}
    	if (ptcb->tcb_code & TCPF_FIN) {
    		newdata++; /* FIN is part of the sequence */
    	}
    }
    //更新下一个要发送的序号
    ptcb->tcb_snext += newdata;
    if (newdata >= 0) {
    	TcpOutSegs++;
    }

    if (ptcb->tcb_state == TCPS_TIMEWAIT) { /* final ACK		*/
        //设置 2MSL计时器
        tcpwait(ptcb);

    }

    datalen += TCP_HLEN(ptcp);

    tcph2net(ptcp);
    ptcp->tcp_cksum = 0;//因为没有得到ip，所以到下一层去设置cksum，
    //ptcp->tcp_cksum = tcpcksum2(ntohl(ptcb->tcb_lip), ntohl(ptcb->tcb_rip), datalen, ptcp);
                 // tcb_rip为网络序
    return ipsend( ptcb->tcb_rip, pep, datalen, IPT_TCP, IPP_NORMAL,IP_TTL);
}


