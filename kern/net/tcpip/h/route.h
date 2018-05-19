#ifndef __KERN_NET_TCPIP_H_ROUTE_H__
#define __KERN_NET_TCPIP_H_ROUTE_H__

struct route {
    IPaddr  rt_net;        /* 该路由的目的地址的网络部分 RT_DEFAULT (默认路由 0.0.0.0) ,RT_LOOPBACK(回环路由) 127.0.0.1 */
    IPaddr  rt_mask;       /* 路由的目的地址掩码             */
    IPaddr  rt_gw;         /* 到rt_net要路过的的下一跳地址               */
    short   rt_metric;     /* 到达目的 rt_net的跳数，  RTM_INF 表示目的路由不可达  */
    short   rt_ifnum;      /* 到下一跳rt_gw，使用的本地网卡号               */
    short   rt_key;        /* sort key                       */
    short   rt_ttl;        /* time to live (seconds)         */
    struct route* rt_next; /* 散列表中 链表连接 */
/* stats */    
	int rt_refcnt;         /* current reference count        */
	int rt_usecnt;         /* total use count so far         */
};

/* Routing Table Global Data: */
struct rtinfo {
    struct route  *ri_default;
    int           ri_bpool;
    bool          ri_valid;
    mutex         ri_mutex;
};
                     //0.0.0.0
#define RT_DEFAULT   ip_anyaddr     /* the default net */
                     //127.0.0.1
#define RT_LOOPBACK  ip_loopback    /* the loopback net			 */
#define RT_TSIZE     512            /* these are pointers; it's cheap */
//永不超时
#define RT_INF       999            /* no timeout for this route  */
//无限远的目的地址，表示不可达
#define RTM_INF      16             /* an infinite metric */

/* rtget()'s second argument... */

#define RTF_REMOTE   0              /* traffic is from a remote host */
#define RTF_LOCAL    1              /* traffic is locally generated  */

#define RT_BPSIZE    100            /* max number of routes      */

/* RTFREE - remove a route reference (assumes ri_mutex HELD)		*/

#define	RTFREE(prt)					\
	if (--prt->rt_refcnt <= 0) {			\
		freebuf(prt);				\
	}

#endif // !__KERN_NET_TCPIP_H_ROUTE_H__


