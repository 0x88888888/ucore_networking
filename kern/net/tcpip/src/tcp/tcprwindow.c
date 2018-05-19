#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcprwindow - do receive window processing for a TCB
 *               在本端的数据发送出去之前，计算本端tcp_window
 *               设置ptcb的tcb_cwin 序号
 *               返回的值为ptcp->tcp_window
 *------------------------------------------------------------------------
 */
int
tcprwindow(struct tcb *ptcb) {
	int window;
    //输入缓冲区的剩余空间
	window = ptcb->tcb_rbsize - ptcb->tcb_rbcount;
    if (ptcb->tcb_state < TCPS_ESTABLISHED) {
        //连接还没完全建立的状况
    	return window;
    }
    /*
     * Receiver-Side Silly Window Syndrone Avoidance:
     * Never shrink an already-advertised window, but wait for at
     * least 1/4 receiver buffer and 1 max-sized segment before
     * opening a zero window.
     */
    if (window*4 < (int)ptcb->tcb_rbsize ||
    	window < (int)ptcb->tcb_rmss) {
        //小于 1/4 或者小于 一个本端的 max segment size ,就发送 0大小的窗口通告
        //使得对方不发送过小的数据
    	window = 0;
    }

    // ptcb->tcb_cwin - ptcb->tcb_rnext == 旧的 window
    window = max(window, ptcb->tcb_cwin - ptcb->tcb_rnext);
                     /* 准备接受的下一个序号 + 接受窗口大小 */    
    ptcb->tcb_cwin = ptcb->tcb_rnext + window;
    return window;
}
