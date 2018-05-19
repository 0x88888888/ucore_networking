#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tfinsert - add a new TCP segment fragment to a TCB sequence queue
 *
 *  将数据插入到ptcb->tcb_rsegq中去
 *------------------------------------------------------------------------
 */
int
tfinsert(struct tcb *ptcb, tcpseq seq, unsigned int datalen) {
	struct tcpfrag  *tf;
	if (datalen == 0) {
		return OK;
	}
	tf= (struct tcpfrag*)kmalloc(sizeof(struct tcpfrag));
	tf->tf_seq = seq;
	tf->tf_len = datalen;
    
    if(ptcb->tcb_rsegq == NULL) {
    	ptcb->tcb_rsegq = create_queue(NTCPFRAG);
    }
    if(enq(ptcb->tcb_rsegq, tf) < 0) {
    	freebuf(tf);
    }
    return OK;
}

