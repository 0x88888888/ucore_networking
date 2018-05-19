#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tfcoalesce -  join TCP fragments
 *
 *  一个完整的tcp数据包接受结束,
 *  将 ptcb->tcb_rsegq 中的数据合并起来
 *------------------------------------------------------------------------
 */
 int
 tfcoalesce(struct tcb *ptcb, unsigned int datalen, struct tcp *ptcp) {
 	struct  tcpfrag  *tf =0;
 	int     newcount;
 	ptcb->tcb_rnext += datalen;
 	ptcb->tcb_rbcount += datalen;
 	if (ptcb->tcb_rnext == ptcb->tcb_finseq) {
 	     goto alldone;
 	}
 	if ((ptcb->tcb_rnext - ptcb->tcb_pushseq) >= 0) {
 	     ptcp->tcp_code |= TCPF_PSH;
 	     ptcb->tcb_pushseq = 0;
 	}
 	if (ptcb->tcb_rsegq == NULL || is_emptyq(ptcb->tcb_rsegq)) {
 	     return OK;
 	}
 	while( (tf->tf_seq - ptcb->tcb_rnext) <= 0 ) {
         newcount = tf->tf_len - (ptcb->tcb_rnext - tf->tf_seq);
         if (newcount > 0) {
    	    ptcb->tcb_rnext += newcount;
    	    ptcb->tcb_rbcount += newcount;
         }
         if (ptcb->tcb_rnext == ptcb->tcb_finseq) {
            	goto alldone;
         }
         if ((ptcb->tcb_rnext - ptcb->tcb_pushseq) >= 0) {
    	    ptcp->tcp_code |= TCPF_PSH;
         	ptcb->tcb_pushseq = 0;
         }
         freebuf(tf);
         tf =(struct tcpfrag *)deq(ptcb->tcb_rsegq);
         if (tf == 0) {
    	    freeq(ptcb->tcb_rsegq);
    	    return OK;
         }
 	}
 	enq(ptcb->tcb_rsegq, tf);
 	return OK;
alldone:
        if (tf){
		freebuf(tf);
        }

	while ( (tf = (struct tcpfrag *)deq(ptcb->tcb_rsegq))){
		freebuf(tf);
        }

	freeq(ptcb->tcb_rsegq);
	ptcp->tcp_code |= TCPF_FIN;
	return OK;
}



