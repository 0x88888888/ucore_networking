#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcprmss - 设置ptcp mss选项
 *------------------------------------------------------------------------
 *
 * tcpsend (只在发送第一个syn的时候调用tcprmss)
 *     tcprmss 
 *
 */
int
tcprmss(struct tcb *ptcb, struct ip *pip) {
	struct tcp *ptcp = (struct tcp *)pip->ip_data;
	int mss, hlen, olen, i;

	hlen = TCP_HLEN(ptcp); 
	olen = 2 + sizeof(short); //4
	//MSS标记
	pip->ip_data[hlen] = TPO_MSS;        /* option kind*/
	//mss数据长度
	pip->ip_data[hlen + 1] = olen;       /* option length */
	//mss值
	mss = ptcb->tcb_rmss;
    //4字节，网络序
	for (i=olen-1; i>1; i--) {
		pip->ip_data[hlen+i] = mss & LOWBYTE;
		mss >>= 8;
	}
	hlen += olen + 3;       /* +3 for proper rounding below */
	/* header length is high 4 bits of tcp_offset, in longs */
	//重新设置 tcp数据开始位置
	ptcp->tcp_offset = ((hlen<<2) & 0xf0) | (ptcp->tcp_offset & 0xf);

    return OK;
}
