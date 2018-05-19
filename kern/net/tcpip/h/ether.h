#ifndef __KERN_NET_TCPIP_H_ETHER_H__
#define __KERN_NET_TCPIP_H_ETHER_H__


/* Ethernet definitions and constants */

#define EP_MINLEN     60               /* minimum packet length        */
#define EP_DLEN       1500             /* length of data field ep      */
#define EP_HLEN       20               /* size of Ether header + length + next hop */
#define EP_MAXLEN     EP_HLEN + EP_DLEN
#define EP_ALEN       6                /* number of octets in physical address */
typedef unsigned char Eaddr[EP_ALEN];  /* length of physical address (48 bits) */
#define EP_RETRY      3                /* number of times to retry xmit errors */
#define	EP_BRC	"\377\377\377\377\377\377" /* Ethernet broadcast address	广播地址 */
#define EP_RTO        300              /* time out in seconds for reads    */

#define EPT_LOOP      0x0060           /* type: Loopback            */
#define EPT_ECHO      0x0200           /* type: Echo                */
#define EPT_PUP       0x0400           /* type: Xerox PUP		    */
#define	EPT_IP		  0x0800           /* type: Internet Protocol	*/
#define EPT_ARP       0x0806           /* type: ARP                 */
#define	EPT_RARP      0x8035           /* type: Reverse ARP		*/

struct  eh {                           /* ethernet header          */
	Eaddr   eh_dst;                    /* destination host address     */
    Eaddr   eh_src;                    /* source host address          */
    unsigned short  eh_type;           /* Ethernet packet type (see below) */
};

#define EH_LEN    (sizeof(struct eh))

struct ep  {                           /* complete struct of Ethernet packet */
     IPaddr ep_nexthop;                /* 在发送之前，用ep_nexthop查找arptable，得到相应的mac地址，这个nexthop是通过查找rttable得到的*/
     short  ep_len;                    /* length of the packet               */
     struct eh ep_eh;                  /* the ethernet header                */
     char   ep_data[EP_DLEN];          /* data in the packet                 */
};

//帧数据头部的额外数据大小
#define EXTRAEPSIZ          6          

/* these allow us to pretend it's all one big happy structure */

//#define	ep_dst	ep_eh.eh_dst
//#define ep_src  ep_eh.eh_src
//#define ep_type ep_eh.eh_type

#define ep_dst(pep)  ((pep)->ep_eh.eh_dst)
#define ep_src(pep)  ((pep)->ep_eh.eh_src)
#define ep_type(pep) ((pep)->ep_eh.eh_type)

#if	Noth > 0
/* this is for Othernet simulation */

struct	otblk {
	bool	ot_valid;	/* these entries are valid?		*/
	Eaddr	ot_paddr;	/* the Othernet physical address	*/
	Eaddr	ot_baddr;	/* the Othernet broadcast address	*/
    int ot_rpid;        /* id of process reading from othernet  */
    int ot_rsem;        /* mutex for reading from the othernet  */
    int ot_pdev;        /* Physical device devtab index     */
    int ot_intf;        /* the associate interface      */
    char *ot_descr;     /* text description of the device   */
};
#endif  //!Noth

/* Ethernet control block description */

struct  etblk   {
	int etdnum;     /* devtab索引  */
	struct device *etdev;    /* devtab中的对象 */
	Eaddr  etpaddr;          /* 网卡物理地址    */
	Eaddr  etbcast;          /* 广播物理地址           */
	int    etrpid;           /* id of process reading from ethernet  */
	int    etrsize;          /* length of data read                  */
    mutex  etrsem;           /* mutex for reading from the etherhnet */
    mutex  etwsem;           /* mutex fro writing to the ethernet    */
    int    etwlen;           /* length of packet bing written or 0   */
    int    extpending;       /* number of packets pending output     */
    int    etrpending;       /* 1 => a receive is pending already    */
    char   *etwbuf;          /* pointer to current transmit buffer   */
    int    etintf;           /* nif中的索引                 */
    char   *etdescr;         /* text description of the device   */
    long   out;		         /* Total output packets */
    long   timeout;          /* Transmitter timeouts */
    long   badtype;          /* Packets with unknown type fields */
    long   drop;             /* Dropped because RX queue too long */
#if Noth > 0
	struct	otblk	*etoth[Noth];	/* eaddr to oaddr translations	*/
#endif	//! Noth    
};

#define ETOUTQSZ 10         /* put this in ether.h   */

extern struct etblk   eth[];

#endif 