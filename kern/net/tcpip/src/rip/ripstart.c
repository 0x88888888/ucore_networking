#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * ripstart - initialize an interface's RIP packet data
 *------------------------------------------------------------------------
 */
 int ripstart(struct rq *prq) {
 	struct ep   *pep;
 	struct ip   *pip;
 	struct udp  *pudp;
 	struct rip  *prip;
 	int pn;

 	pn = ++prq->rq_cur;
 	if (pn >= MAXNRIP){
 		return SYSERR;
 	}

 	prq->rq_nrts = 0;
 	prq->rq_pep[pn] = pep = (struct ep*)kmalloc(sizeof(struct ep));

 	if (pep ==  NULL) {
 		return SYSERR;
 	}

 	pip = (struct ip*)pep->ep_data;
 	pudp = (struct udp*)pip->ip_data;
 	prip = (struct rip*)pudp->u_data;

 	prq->rq_prip = prip;
 	prq->rq_len[pn] = RIPHSIZE;
 	prip->rip_cmd = RIP_RESPONSE;
 	prip->rip_vers = RIP_VERSION;
    prip->rip_mbz = 0;
    return OK;
 }

 
