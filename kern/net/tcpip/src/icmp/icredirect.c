#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  icredirect -  根据icmp重定向信息(pep中)修改本机路由表
 *------------------------------------------------------------------------
 */
 int icredirect(struct ep *pep) {
 	struct route  *prt;
 	struct ip      *pip, *pip2;
 	struct icmp    *pic;
 	IPaddr		mask;

 	pip = (struct ip*)pep->ep_data;
 	pic = (struct icmp*)pip->ip_data;
 	pip2= (struct ip*)pic->ic_data; 

 	if (pic->ic_code == ICC_HOSTRD) {
 	   //主机重定向 mask = ff:ff:ff:ff
       mask= ip_maskall;
 	} else {
 	   //网络重定向  mask = netmask(ip_dst)
 	   mask=netmask( pip2->ip_dst);
 	}
 	//获取需要修改的重定向的路由信息
 	prt = rtget(pip2->ip_dst, RTF_LOCAL);
 	if(prt == 0) {
 		freebuf(pep);
 		return OK;
 	}

 	if(pip->ip_src ==prt->rt_gw) {
        //除去老的
 		rtdel(pip2->ip_dst, mask);
 		//添加新的
 		rtadd(pip2->ip_dst, mask, pic->ic_gw, prt->rt_metric, prt->rt_ifnum, IC_RDTTL);
 	}
 	rtfree(prt);
 	freebuf(pep);
    return OK;
 }


