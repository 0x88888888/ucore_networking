#include <tcpip/h/network.h>

int rtdel(IPaddr net, IPaddr mask) {
    struct route *prt, *prev;
    int     hv;

    if (!Route.ri_valid) {
    	return SYSERR;
    }
    lock(&Route.ri_mutex);
    if(Route.ri_default &&
    	(net == Route.ri_default->rt_net)) {
    	RTFREE(Route.ri_default);
        Route.ri_default = NULL;
        unlock(&Route.ri_mutex);
        return OK;
    }
    hv = rthash(net);

    prev = NULL;
    for (prt = rttable[hv]; prt; prt = prt->rt_next) {
    	if ((net == prt->rt_net) &&
    		(mask== prt->rt_mask)){
    		break;
    	}
    	prev=prt;
    }
    if (prev == NULL) {
    	unlock(&Route.ri_mutex);
    	return SYSERR;
    }
    if(prev) {
    	prev->rt_next = prt->rt_next;
    } else {
    	rttable[hv] = prt->rt_next;
    }
    RTFREE(prt);
    unlock(&Route.ri_mutex);
    return OK;
}

