#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * ripsend - send a RIP update
 *
 * gw: remote gateway (FFFFFFFF => all)
 * port:  remote port
 *------------------------------------------------------------------------
 */
int ripsend(IPaddr gw, unsigned short port) {
   
   struct rq *prq, rqinfo[NIF];
   struct route  *prt;
   int i , pn;

   if (ripifset(rqinfo, gw, port) != OK)
   	return SYSERR;

   lock(&Route.ri_mutex);
   for(i=0; i<RT_TSIZE; i++) {
   	  for(prt=rttable[i]; prt; prt= prt->rt_next)
   	  	ripadd(rqinfo, prt);
   }
   if (Route.ri_default) {
   	    ripadd(rqinfo, Route.ri_default);
   }
   unlock(&Route.ri_mutex);
  
   for (i=0; i<Net.nif; ++i){
		if (rqinfo[i].rq_active) {
			prq = &rqinfo[i];
			for (pn=0; pn<=prq->rq_cur; ++pn)
				udpsend(prq->rq_ip, prq->rq_port, UP_RIP,
                    prq->rq_pep[pn], prq->rq_len[pn],1);
		}
	}
   return OK;
}

