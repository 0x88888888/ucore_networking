#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpsndlen - 返回即将可以发送的数据量
 *------------------------------------------------------------------------
 *   
 *
 *  tcpidle
 *     tcpxmit
 *  tcpout 
 *     tcpxmit
 *     tcprexmt
 *     tcppersist
 *         tcpsend
 *             tcpsndlen
 */
int tcpsndlen(struct tcb *ptcb, bool rexmt, unsigned int *poff) {
	//struct uqe   *puqe, *puqe2;
	unsigned int  datalen;

	if (rexmt || (ptcb->tcb_code & TCPF_SYN)) {
		//sbsnd_buf中的开始位置
		*poff = 0;
	} else {
		//没有被确认的数据量
		*poff = ptcb->tcb_snext - ptcb->tcb_suna;
	}
	//发送缓冲区中个可以发出去的数据量
	datalen = ptcb->tcb_sbcount - *poff;
	//发送的数据量不能超过对端的通告窗口
	datalen = min(datalen, ptcb->tcb_swindow);
    
    //同样，发送的数据量，不能超过对端的max segment size
	return min(datalen, ptcb->tcb_smss);
}


