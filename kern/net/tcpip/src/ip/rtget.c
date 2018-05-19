#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  rtget  -  get the route for a given IP destination
 *
 *  dest: 目的地网络地址或者主机地址
 *  local: 数据包是否有本机产生
 *------------------------------------------------------------------------
 */        
struct route *rtget(IPaddr dest, bool local /* 报文是有本地还是外部主机生成 */){
	struct route *prt;
	int           hv;

	if (!Route.ri_valid){
		rtinit();
	}
	lock(&Route.ri_mutex);
	hv=rthash(dest);

	for(prt=rttable[hv]; prt; prt=prt->rt_next) {
		if (prt->rt_ttl <= 0) {
			continue;    /* 已经过期的，等待删除 */
		}
		//dest是否在网络rt_net中
		if (netmatch(dest, prt->rt_net, prt->rt_mask, local)) {
			//找对对应的路由项
			if (prt->rt_metric < RTM_INF) {
				break;
			}
		}
	}

    if (prt == NULL) {
    	// 返回默认路由
        prt = Route.ri_default; //Route.ri_default 也可能是NULL
    }
    if (prt != NULL && prt->rt_metric >= RTM_INF) {
    	prt= NULL;
    }
    if(prt) {
    	prt->rt_refcnt++;
    	prt->rt_usecnt++;
    }
    unlock(&Route.ri_mutex);
    return prt;
}


