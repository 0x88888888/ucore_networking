#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ripadd  -  add the given route to the RIP packets yet to send
 *------------------------------------------------------------------------
 */
int ripadd(struct rq rqinfo[], struct route* prt) {
	IPaddr  net;
	int i, metric, pn, rn;
    
    for (i=0; i<Net.nif; i++) {
    	struct rq *prq = &rqinfo[i];
    	struct riprt   *rp;

    	if (!rqinfo[i].rq_active || nif[i].ni_state != NIS_UP){
            continue;
    	}

    	metric = ripmetric(prt, i);
    	if (metric == SYSERR) 
    		continue;
    	if (prq->rq_nrts >= MAXRIPROUTES &&
    		ripstart(prq) != OK) {
    		continue;
    	}
    	pn = prq->rq_cur;
    	rn = prq->rq_nrts++;
    	rp = &prq->rq_prip->rip_rts[rn];
    	rp->rr_family = htons(AF_INET);
    	rp->rr_mbz = 0;
    	net = netnum(prt->rt_net);
    	bzero(rp->rr_pad, sizeof(rp->rr_pad));
    	if ((nif[i].ni_net == net) ||
    		(prt->rt_mask == ip_maskall)) {
    		rp->rr_ipa = prt->rt_net;
    	} else { /* send the net part only (esp. for subnets)	*/
            rp->rr_ipa = net;
    	}
    	rp->rr_metric = htonl(metric);
    	prq->rq_len[pn] += sizeof(struct riprt);
    }
    return OK;
}

