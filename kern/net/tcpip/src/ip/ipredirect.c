#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ipredirect  -  send redirects, if needed
 *  pep:    the current IP packet
 *  ifnum:  the input interface
 *  prt:    where we want to route it
 *------------------------------------------------------------------------
 */
void ipredirect(struct ep *pep, int ifnum, struct route *prt){
	struct ip *pip = (struct ip*)pep->ep_data;
	struct route *tprt;
	int rdtype, isonehop;
	IPaddr nmask;   /* network part's mask */

    if (ifnum == NI_LOCAL || ifnum != prt->rt_ifnum){
    	return;
    }

    tprt = rtget(pip->ip_src , RTF_LOCAL);
    if(!tprt) {
    	return;
    }
    //判断发送方是否和路由在同一个网段中
    isonehop = tprt->rt_metric == 0;
    if (!isonehop){
        //不在同一个网络中，不能发送icmp重定向信息
    	return;
    }
    //计算网络掩码
    nmask = netmask( prt->rt_net);/* get the default net mask	*/
    if (prt->rt_mask == nmask){
    	rdtype = ICC_NETRD;
    } else {
    	rdtype = ICC_HOSTRD;
    }
    icmp(ICT_REDIRECT, rdtype, pip->ip_src, pep, (void *)prt->rt_gw);
}
