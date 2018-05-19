#ifndef __KERN_NET_TCPIP_H_ARP_H__
#define __KERN_NET_TCPIP_H_ARP_H__ 
/* Internet Address Resolution Protocol  (see RFCs 826, 920)		*/

#define  AR_HARDWARE    1     /* Ethernet hardware type code     */

/* Definitions of codes used in operation field of ARP packet */

#define  AR_REQUEST     1     /* ARP request to resolve address */
#define  AR_REPLY       2     /* reply to a resolve request     */

#define  RA_REQUEST     3     /* reverse ARP request (RARP packets) */
#define  RA_REPLY       4     /* reply to a reverse request (RARP)  */

struct  arp {
    unsigned short ar_hwtype;          /* hardware type               */
    unsigned short ar_prtype;          /* protocol type               */
    unsigned char  ar_hwlen;           /* hardware address length     */
    unsigned char  ar_prlen;           /* protocol address length     */
    unsigned short ar_op;              /* ARP operation (see list above) */
    unsigned char  ar_addrs[1];        /* sender and target hw & proto addrs */
    /*	char	ar_sha[6];	  // sender's physical hardware address	*/
    /*	char	ar_spa[4];	  // sender's protocol address (IP addr.)	*/
    /*	char	ar_tha[6];	  // target's physical hardware address	*/
    /*	char	ar_tpa[4];	  // target's protocol address (IP)	*/
};

// source hardware address
#define  SHA(p)   (&p->ar_addrs[0])
// source protocol address
#define  SPA(p)   (&p->ar_addrs[p->ar_hwlen])
// target hardware address
#define THA(p)    (&p->ar_addrs[p->ar_hwlen + p->ar_prlen])
// target protocol address
#define	TPA(p)	(&p->ar_addrs[(p->ar_hwlen*2) + p->ar_prlen])

#define MAXHWALEN   EP_ALEN	/* Ethernet				*/
#define	MAXPRALEN	IP_ALEN	/* IP					*/

#define ARP_HLEN    8   /* ARP header length            */

#define ARP_TSIZE   50   /* ARP cache size */
#define ARP_QSIZE   10   /* ARP port queue size */

/* cache timeouts 秒为单位 */
#define ARP_TIMEOUT	(600 * 100)	/* 10 minutes				*/
#define ARP_INF     0x7fffffff  /* "infinite" timeout value */
#define	ARP_RESEND	    (1*100)	/* resend if no reply in 1 sec		*/
#define	ARP_MAXRETRY	4	/* give up after a ~30 seconds		*/

struct  arpentry {       /* format of entry in ARP cache		*/
    short ae_state;      /* state of this entry (see below)     */
    
    //下面的四个字段对应arp头的前面四项
    short ae_hwtype;     /* hardware type                       */
    short ae_prtype;     /* protocol type                       */
    char  ae_hwlen;	     /* hardware address length		*/
    char  ae_prlen;   /* protocol address length      */

    struct netif *ae_pni; /* pointer to interface structure */
    /* 输出队列，就是等待发送的各种上层协议的数据包 queue of packets for this address */
    queue*	ae_queue;	
    int	ae_attempts;	/* 记录这个arp请求重发的次数	*/
    int	ae_ttl;		/* time to live				*/
    char	ae_hwa[MAXHWALEN];	/* Hardware address 		*/
	char	ae_pra[MAXPRALEN];	/* Protocol address 		*/
};

#define	AS_FREE		0	/* Entry is unused (initial value)	*/
//arp请求已经发出，但是还没有收到回复
#define	AS_PENDING	1	/* Entry is used but incomplete		*/
#define	AS_RESOLVED	2	/* Entry has been resolved		*/

/* RARP variables */
extern int	rarppid;	/* id of process waiting for RARP reply	*/
extern mutex rarpsem;	/* semaphore for access to RARP service	*/

extern struct   arpentry    arptable[ARP_TSIZE];

extern void arpinit();

#endif // !__KERN_NET_TCPIP_H_ARP_H__
 
