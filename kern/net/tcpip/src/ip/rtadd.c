#include <tcpip/h/network.h>


/*------------------------------------------------------------------------
 *  rtadd  -  rttable中添加 一条路由信息
 *------------------------------------------------------------------------
 */
int rtadd(IPaddr net, IPaddr mask, IPaddr gw, int metric, int intf, int ttl) {
	struct route *prt, *srt, *prev;
	bool   isdup;
	int  hv, i ;

	if (!Route.ri_valid) {
		rtinit();
	}
	
    //填充得到一个新的路由信息
	prt = rtnew(net, mask, gw, metric, intf, ttl);

	if (prt == (struct route *)SYSERR) {
		//unlock(&Route.ri_mutex);
		return SYSERR;
	}

	/* compute the queue sort key for this route */
    prt->rt_key = 0;
    for (i=0; i<32; ++i){
        prt->rt_key += (mask >> i) & 1;
    }
    
    lock(&Route.ri_mutex);
	/* special case for default routes */
	if (net == RT_DEFAULT ) {
        // 修改默认路由
		if(Route.ri_default) {
			RTFREE(Route.ri_default);
		}
        if (prt->rt_gw == (IPaddr)0) {
            panic("rtadd error: rt_gw == 0\n");
        }
		Route.ri_default = prt;
		unlock(&Route.ri_mutex);
		return OK;
	}
	prev = NULL;
	hv = rthash(net);
	isdup =false;
	for(srt = rttable[hv]; srt; srt=srt->rt_next) {
		if (prt->rt_key > srt->rt_key) {
			break;
		}
		if ((srt->rt_net == prt->rt_net) && 
			(srt->rt_mask== prt->rt_mask) ) {
            //路由表中已经有记录了
			isdup= true;
		    break;
		}
		prev = srt;
	}

    if (isdup) {
        //原本就有一条如何rt_net的记录了
    	struct route *tmprt;

    	if ((srt->rt_gw == prt->rt_gw)) {
            //下一条地址又相等，更新路由信息
            /* just update the existing route */
            if(dorip) {
            	srt->rt_ttl = ttl;
            	if (srt->rt_metric != metric) {
            		if (metric == RTM_INF) {

            			srt->rt_ttl =RIPZTIME;
            		}
                    //已经修改路由信息，需要通知rip进程，让rip进程广播到别的路由器中去
            		panic(" ip/rtadd.c rtadd not be implemented fully 1\n");
            	}
            }
            //设置新的过期时间
            srt->rt_metric = metric;
            RTFREE(prt);
            unlock(&Route.ri_mutex);
            return OK;
    	}
    	/* else, someone else has a route there... */
    	if  (srt->rt_metric <= prt->rt_metric) {
            //舍近求远，不要
    		/* no better off to change; drop the new one */
    		RTFREE(prt);
    		unlock(&Route.ri_mutex);
    		return OK;
    	} else if (dorip) {
            panic(" ip/rtadd.c rtadd not be implemented fully 2\n");
    	}
    	tmprt = srt;
    	srt = srt->rt_next;
    	RTFREE(tmprt);
    } else if (dorip) {
    	panic(" ip/rtadd.c rtadd not be implemented fully 3\n");
    }
    //新路由prt，进入rttable
    prt->rt_next = srt;
    if (prev) {
    	prev->rt_next = prt;
    } else {
    	rttable[hv] = prt;
    }
    unlock(&Route.ri_mutex);
    return OK;
}


