#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpwakeup -  wake up processes sleeping for TCP, if necessary
 *	NB: Called with tcb_mutex HELD
 *------------------------------------------------------------------------
 *
 * 唤醒等在ptcb->tcb_rsema或 ptcb->tcb_ssema上的进程
 *
 *
 *  tcpabort
 *  tcpdodat
 *  tcpgetspace
 *  tcpread
 *  tcpwr
 *     tcpwakeup
 */
int
tcpwakeup(int type, struct tcb *ptcb) {
	int     freelen;
	int     intr_flag;
	local_intr_save(intr_flag);
	if (type & READERS) {
		if (( (ptcb->tcb_flags & TCBF_RDONE)||
		      ptcb->tcb_rbcount>0 ||
		      (ptcb->tcb_flags & TCBF_RUPOK)) &&
			   sem_wait_count(&(ptcb->tcb_rsema)) > 0
			/*scount(&(ptcb->tcb_rsema)) <=0*/) {
		    /* 1,对方发出 FIN
		     * 2,接收缓冲区中有数据没有被接收
		     * 3,收到urgent数据
		     * 4,并且有进程等待在tcb_rsema上
		     */
			signal(&(ptcb->tcb_rsema));
		}
	}
	if (type & WRITERS) {
		freelen = ptcb->tcb_sbsize - ptcb->tcb_sbcount;
		if (((ptcb->tcb_flags & TCBF_SDONE) || freelen > 0) &&
			sem_wait_count(&(ptcb->tcb_ssema)) > 0
		    /*scount(&(ptcb->tcb_ssema)*/ )
		    /* 1,不再发送数据
		     * 2,发送缓冲区有空闲空间
		     * 3,并且有进程等待在tcb_ssema上
		     */
			signal(&(ptcb->tcb_ssema));
		/* special for abort */
		if (ptcb->tcb_error && sem_val(&(ptcb->tcb_ocsem)) > 0)
			signal(&(ptcb->tcb_ocsem));
	}
	local_intr_restore(intr_flag);
	return OK;
}

