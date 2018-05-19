#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpclose - 关闭一个tcp连接
 *------------------------------------------------------------------------
 */
int
tcpclose(struct device* pdev) {
	struct  tcb  *ptcb = (struct tcb*)pdev->dvioblk;
	int     error;
    
    lock(&(ptcb->tcb_mutex));
    switch(ptcb->tcb_state) {
    	case TCPS_LISTEN:
    	case TCPS_ESTABLISHED:
    	case TCPS_CLOSEWAIT:
    	    break;
    	case TCPS_FREE:
    	    return SYSERR;
    	default:
    	    unlock(&(ptcb->tcb_mutex));
    	    return SYSERR;
    }

    if (ptcb->tcb_error || ptcb->tcb_state == TCPS_LISTEN) {
        //有错误信息，和LISTEN状态的tcb,只在本地处理
    	return tcbdealloc(ptcb);
    }

    /* to get here, we must be in ESTABLISHED or CLOSE_WAIT */
    TcpCurrEstab--;
    //不再发送数据
    ptcb->tcb_flags |= TCBF_SNDFIN;

    ptcb->tcb_slast = ptcb->tcb_suna + ptcb->tcb_sbcount;
    if(ptcb->tcb_state == TCPS_ESTABLISHED) {
        //主动关闭的一方，进入TCPS_FIN_WAIT1状态
    	ptcb->tcb_state = TCPS_FINWAIT1;
    } else { /* CLOSE_WAIT */
        //被动关闭的一方，进入LAST_ACK状态
        ptcb->tcb_state = TCPS_LASTACK;
    }
    //输出
    ptcb->tcb_flags |= TCBF_NEEDOUT;
    tcpkick(ptcb);
    unlock(&(ptcb->tcb_mutex));
    wait(&(ptcb->tcb_ocsem)); /* wait for FIN to be ACKed	*/
    error = ptcb->tcb_error;
    if (ptcb->tcb_state == TCPS_LASTACK) {
        //被动关闭的一方，直接dealloc掉
    	tcbdealloc(ptcb);
    }
    return error;
}


