#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ipfcons  -  construct a single packet from an IP fragment queue
 *------------------------------------------------------------------------
 */
struct ep* ipfcons(struct ipfq* iq) {
	struct ep*    pep;
	struct ep*    peptmp;
	struct ip*    pip;
    int      off, seq;

    pep = (struct ep*)kmalloc(sizeof(struct ep));
    if (pep == (struct ep*)SYSERR) {
        //内存分配失败，丢弃掉原先的所有分片
    	while( (peptmp = (struct ep*)deq(iq->ipf_q)) ) {
    		IpReasmFails++;
    		freebuf(peptmp);
    	}
    	assert(is_emptyq(iq->ipf_q));
    	iq->ipf_state = IPFF_FREE;
    	return 0;
    }
    /* copy the Ether and IP headers */

    peptmp = (struct ep*)deq(iq->ipf_q);
    pip = (struct ip*)peptmp->ep_data;
    off = IP_HLEN(pip);
    seq = 0;
    blkcopy(pep, peptmp, EP_HLEN+off); 
    
    /* 数据复制回来 */
    while(peptmp != 0) {
    	int dlen, doff;

    	pip = (struct ip*)peptmp->ep_data;
    	doff = IP_HLEN(pip) + seq - ((pip->ip_fragoff&IP_FRAGOFF)<<3);
    	dlen = pip->ip_len - doff;
    	blkcopy(pep->ep_data+off, peptmp->ep_data+doff, dlen);
    	off += dlen;
    	seq += dlen;
        freebuf(peptmp);
        peptmp = (struct ep *)deq(iq->ipf_q);
    }

    /* 重新设置fragoff和ip_len */
    pip = (struct ip*)pep->ep_data;
    pip->ip_len = off;
    pip->ip_fragoff = 0;

    /* 队列空出来 */
    assert(iq->ipf_q);
    iq->ipf_state = IPFF_FREE;
	IpReasmOKs++;
    return pep;
}

