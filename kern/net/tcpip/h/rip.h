#ifndef __KERN_NET_TCPIP_H_RIP_H__
#define __KERN_NET_TCPIP_H_RIP_H__
#define RIPHSIZE         4    /* size of the header in octets */
#define RIP_VERSION      1    /* rip version number           */
#define AF_INET          2    /* address family for ip        */

/* rip commands codes:  */

#define  RIP_REQUEST    1     /* message request routes       */
#define  RIP_RESPONSE   2     /* message contains route advertisement */

/* 一条rip 路由信息*/

struct riprt {
    short   rr_family;        /* 4BSD Address Family          */
    short   rr_mbz;           /* must be 0 */
    //char    rr_addr[12];      /* the part we use for ip:(0-3) */
    unsigned long rr_ipa;     /* the part we use for IP     */
    char    rr_pad[8];  /* remaining address for non-IP     */
    long    rr_metric;        /* distance (hop count) metric      */
};

#define MAXRIPROUTES      25   /* MAX routes per packet   */
#define RIP_INFINITY      16   /* dest is unreachable     */

#define RIPINT		300	/* interval to send (1/10 secs)		*/
#define	RIPDELTA	 50	/* stray RIPINT +/- this (1/10 secs)	*/
#define	RIPOUTMIN	 50	/* min interval between updates (1/10 s)*/

#define RIPRTTL		180	/* route ttl (secs)			*/
#define	RIPZTIME	120	/* zombie route lifetime (secs)		*/ 

/* rip 包结构 */
struct rip {
    /*
     RIP_REQUEST或者RIP_RESPONSE
    */
    char rip_cmd;    /* rip command */
    char rip_vers;   /* rip_version, above */
    short rip_mbz;   /* 用0填空 */
    /* rip协议的数据部分 */
    struct riprt   rip_rts[MAXRIPROUTES];
};

/* RIP process definitions */
extern  int32_t rip(void* arg);

#define MAXNRIP    5     /* max # of packets per update	*/

/* Per-interface RIP send data */
struct rq {
    bool    rq_active;         /* true=> this data is active */
    IPaddr  rq_ip;             /* destination IP address     */
    unsigned short rq_port;    /* destination port number	*/
/* what we've built already:		*/
    struct ep *rq_pep[MAXNRIP]; /* packets to send */
    int    rq_len[MAXNRIP];     /* length of each */
/* in-progress packet information	*/    
	int    rq_cur;             /* current working packet	*/
	int    rq_nrts;            /* # routes this packet		*/
	struct rip *rq_prip;       /* current rip data		*/
};

extern  mutex  riplock;
extern	bool   dorip;			/* TRUE => do active RIP	*/
extern  int    rippid;

#endif //!__KERN_NET_TCPIP_H_RIP_H__