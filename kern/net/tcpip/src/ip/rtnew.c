#include <tcpip/h/network.h>

struct route* rtnew(IPaddr net, IPaddr mask, IPaddr gw,
	                int mertric, int ifnum, int ttl)
{
	struct route *prt;

	prt = (struct route*)kmalloc(sizeof(struct route));
	if (prt == NULL) {
		return (struct route*) SYSERR;
	}

	prt->rt_net = net;
	prt->rt_mask =mask ;
	prt->rt_gw=gw;
	prt->rt_metric = mertric;
	prt->rt_ifnum = ifnum;
	prt->rt_ttl = ttl;
	prt->rt_refcnt = 1;  /* our caller */
	prt->rt_usecnt = 0;
	prt->rt_next = NULL;
	return prt;
}


