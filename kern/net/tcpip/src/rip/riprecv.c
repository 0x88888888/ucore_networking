#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  riprecv  -  process a received RIP advertisement
 *------------------------------------------------------------------------
 */
int riprecv(struct rip* prip, int len, IPaddr gw) {
	struct  route  *prt;
	IPaddr  mask;
	int nrts, rn, ifnum;
	short rr_metric;

	nrts = (len - RIPHSIZE) / sizeof(struct riprt);
	prt = (struct route *)rtget(gw, RTF_REMOTE);   /* find the interface number	*/
	if (prt == NULL) {
		return SYSERR;
	}
	ifnum = prt->rt_ifnum;
	rtfree(prt);
	lock(&riplock);
	for (rn= 0; rn<nrts; ++rn) {
		struct riprt    *rp = &prip->rip_rts[rn];
		rp->rr_family = ntohs(rp->rr_family);
		rp->rr_metric = ntohs(rp->rr_metric);
		if (!ripok(rp)) {
			continue;
		}
		mask=netmask( rp->rr_ipa);
		rr_metric = rp->rr_metric;    /* rtadd wants a short */
		rtadd(rp->rr_ipa, mask, gw, rr_metric,  /* was rp->rr_metric   */
		      ifnum, RIPRTTL);
	}
    unlock(&riplock);
    return OK;
}

