#include <tcpip/h/network.h>

/*----------------------------------------------------------------
 * tcpsmss - set sender MSS from option in incoming segment
 *  根据tcp的option数据得到 对端的max segment size
 *  tcpinp
 *     tcpopts
 *        tcpsmss
 *----------------------------------------------------------------
 */
int
tcpsmss(struct tcb *ptcb, struct tcp *ptcp, unsigned char *popt) {
	unsigned int mss, len;
    
	len = *++popt;
	++popt; /* skip length field */
	if ((ptcp->tcp_code & TCPF_SYN) == 0) {
		//mss只在三次握手过程中协商，除此之外的，都忽略
		return len;
	}
	//读取mss
	switch (len -2) { /* subtract kind & len	*/
	    case sizeof(char):
	       mss = *popt;
	       break;
	    case sizeof(short):
	       mss = ntohs(*(unsigned short *)popt);
	       break;
	    case sizeof(long):
	       mss = ntohl(*(unsigned short *)popt);
	       break;
	    default:
	       mss = ptcb->tcb_smss;
	       break;
	}
	/* 
	 * 去掉tcp头
	 * 只保留可接收数据的 max segment size
	 * 
	 */
	mss -= TCPMHLEN;   
	//记录下对端的 max segment size
	if(ptcb->tcb_smss) {
		//取小的
		ptcb->tcb_smss = min(mss, ptcb->tcb_smss);
	}else {
		//直接设置
		ptcb->tcb_smss = mss;
	}
	return len;
}

