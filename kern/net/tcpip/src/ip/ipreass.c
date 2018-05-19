#include <tcpip/h/network.h>

mutex ipfmutex;
struct	ipfq	ipfqt[IP_FQSIZE];

/*------------------------------------------------------------------------
 *  ipreass  -  重组ip数据的分片,判断收到的帧是否是一个分片，如果是，那就放入重组队列
 *	returns packet, if complete; 0 otherwise
 *------------------------------------------------------------------------
 */
struct ep* ipreass(struct ep *pep) {
    struct  ep *pep2;//重组后的数据
    struct  ip *pip;
    int   firstfree;
    int   i;

    pip = (struct ip*)pep->ep_data;
    lock(&ipfmutex);

    if ( (pip->ip_fragoff & (IP_FRAGOFF | IP_MF)) ==0) {
        //不需要分片
    	unlock(&ipfmutex);
    	return pep;
    }
    IpReasmReqds++;
    firstfree = -1;

    for(i=0; i<IP_FQSIZE; i++) {
    	struct ipfq   *piq = &ipfqt[i];
        
        if (piq->ipf_state == IPFF_FREE) {
            if(firstfree == -1) {
            	firstfree = i;
            }
            continue;
        }
        if (piq->ipf_id != pip->ip_id) {
        	continue;
        }
        if (piq->ipf_src == pip->ip_src) {
        	continue;
        }
        /* found a match */
        if (ipfadd(piq, pep) == 0) {
            //添加到 piq->ipf_q
        	unlock(&ipfmutex);
        	return 0;
        }
        //队列中分片是否到齐，到齐就重组
        pep2 = ipfjoin(piq);
        unlock(&ipfmutex);
        return pep2;
    }

    //没有找到合适的分片队列
    /* no match */

    if (firstfree < 0) {
        //没有空闲的队列可供使用，丢弃包
    	/* no room-- drop */
    	freebuf(pep);
    	unlock(&ipfmutex);
    	return 0;
    }
    
    //有可供使用的队列
    if (ipfqt[firstfree].ipf_q == NULL){
        ipfqt[firstfree].ipf_q = create_queue(20);
        if(ipfqt[firstfree].ipf_q == NULL) {
            freebuf(pep);
            unlock(&ipfmutex);
            return 0;
        }
    }
    
    //将分片放入队列
    ipfqt[firstfree].ipf_src = pip->ip_src;
    ipfqt[firstfree].ipf_id = pip->ip_id;
    ipfqt[firstfree].ipf_ttl = IP_FTTL;
    ipfqt[firstfree].ipf_state = IPFF_VALID;
    ipfadd(&ipfqt[firstfree], pep);
    unlock(&ipfmutex);
    return 0;
}
