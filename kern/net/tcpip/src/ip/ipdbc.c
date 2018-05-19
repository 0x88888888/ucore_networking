#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * ipdbc - 定向广播处理
 *         主机发送一个定向广播
 *         主机接受一个定向广播
 *         主机转发一个定向广播
 *
 *
 *------------------------------------------------------------------------
 */
 void ipdbc(int ifnum, struct ep *pep, struct route* prt) {
 	struct ip *pip = (struct ip*) pep->ep_data;
 	struct ep *pep2=NULL;
 	struct route *prt2=NULL; //指向定向广播路由
 	int len;

    /*
     * 下一跳不是交付路由，说明还没到达目的网络，不处理
     */
 	if (prt->rt_ifnum != NI_LOCAL){
 		return;    /* not ours */
    }
    
    //是否是广播地址
    if (!isbrc(pip->ip_dst))
        return;  /* not broadcast */
    //程序执行到这里说明需要在本网络进行广播,获取针对定向广播地址的路由(转发路由)
    prt2 = rtget(pip->ip_dst, RTF_LOCAL /*1*/);

    if(prt2 == NULL)
    	return ;
    if(prt2->rt_ifnum == ifnum) { /* not directed	*/
        rtfree(prt2);
        return;
    }
    //至此往后的代码，完全用于收到定向广播报文的最后一跳路由器对报文的转发
    /* directed broadcast; make a copy */

    /* len= ether header + IP packet */

    len = EP_HLEN + pip->ip_len; /* test +6 */
    pep2 = (struct ep*)kmalloc(sizeof(struct ep));
    /*
    if (len > EP_MAXLEN){
    	pep2 = (struct ep*)kmalloc(sizeof(struct ep));
    } else {
    	pep2 = (struct ep*)kmalloc(sizeof(struct ep));
    }
    */
    if (pep2 == NULL) {
    	rtfree(prt2);
    	return;
    }
    blkcopy(pep2, pep, len);
    /* send a copy to the net */
    ipputp(prt2->rt_ifnum, pip->ip_dst, pep2);
    rtfree(prt2);
    return ;  
 	//panic("ipdbc not be implemented fully\n");
 }