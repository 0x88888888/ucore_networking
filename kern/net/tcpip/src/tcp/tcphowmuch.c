#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcphowmuch.c - compute how much data is available to send
 *------------------------------------------------------------------------
 * 计算出可以发送的空间大小
 */
int
tcphowmuch(struct tcb *ptcb) {
	int tosend;
    /* 算出可以发送的数据大小 */
	tosend = ptcb->tcb_suna + ptcb->tcb_sbcount - ptcb->tcb_snext;
	if (ptcb->tcb_code & TCPF_SYN) {
		//要发送syn标记，这个标记占一个位置
		++tosend;
	}
	if(ptcb->tcb_flags & TCBF_SNDFIN) {
		//要发送fin标记，这个标记也占一个位置
		++tosend;
	}
	return tosend;
}


