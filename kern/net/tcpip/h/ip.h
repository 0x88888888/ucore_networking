#ifndef __KERN_NET_TCPIP_H_IP_H__
#define __KERN_NET_TCPIP_H_IP_H__ 

#define BPMAXB   2048
#define IP_ALEN  4    /* IP address length in bytes (octets)	*/
//typedef	unsigned char IPaddr[IP_ALEN];	/*  internet address	*/
typedef unsigned long IPaddr;

#define IP_CLASSA(x)  (((x) & 0x80) == 0x00)  /* IP Class A address */
#define IP_CLASSB(x)  (((x) & 0xc0) == 0x80)  /* IP Class B address */
#define IP_CLASSC(x)  (((x) & 0xe0) == 0xc0)  /* IP Class C address */
#define IP_CLASSD(x)  (((x) & 0xf0) == 0xe0)  /* IP Class D address */
#define IP_CLASSE(x)  (((x) & 0xf8) == 0xf0)  /* IP Class E address */

#define IP_SAMEADDR(ip1,ip2) (*((int *) ip1) == *((int *) ip2))
#define IP_COPYADDR(ipto,ipfrom) (*((int *) ipto) = *((int *) ipfrom))

/* Some Assigned Protocol Numbers */

#define  IPT_ICMP      1  /* protocol type for ICMP packets */
#define  IPT_IGMP      2   /* protocol type for IGMP packets   */
#define  IPT_TCP       6  /* protocol type for TCP  packets */
#define  IPT_EGP       8  /* protocol type for EGP packets	*/
#define  IPT_UDP       17 /* protocol type for UDP packets	*/
#define  IPT_OSPF      89  /* protocol type for OSPF packets   */
/**********************************************************************

                            IP 头格式

-------- --------   ---------------- ---------------- ----------------
  版本      长度        tos                        ip_len
-------- --------   ----------------| ------|---------- ----------------
       ip_id                        | flag  |    ip_fragoff
-------- --------   ----------------| ------|---------- ----------------
     ip_ttl             ip_proto                  ip_chksum
-------- --------   ---------------- ---------------- ----------------
               ip_src                             ip_dst
-------- --------   ---------------- ---------------- ----------------

***********************************************************************/
struct  ip  {
    unsigned char    ip_verlen;    /* ip头部长度 IP version & header length (in longs)  */
    unsigned char    ip_tos;       /* type of service			*/
    unsigned short   ip_len;       /* 总长度，包括ip头长度和ip数据的长度 total packet length (in octets) */
    short   ip_id;        /* datagram id */ 
    short   ip_fragoff;   /* fragment offset (in 8-octet's) */
    unsigned char    ip_ttl;       /* time to live, in gateway hops */ 
    unsigned char    ip_proto;     /* IP protocol (set IPT_ above) */
    short   ip_cksum;     /* 计算这个值的时候，ip头已经处于网络序 */
    IPaddr  ip_src;       /* IP address of source */
    IPaddr  ip_dst;       /* IP address of destination */
    unsigned char    ip_data[1];   /* variable length data */
};

#define  IP_VERSION   4   /* current version value */
#define  IP_MINHLEN   5   /* minimum IP header length (in longs) */
#define  IP_TTL       255  /* Initial time-to-live value       */


#define  IP_MF        0x2000  /* more fragments bit  */
#define  IP_DF        0x4000  /* don't fragment bit  */
#define  IP_FRAGOFF   0x1fff  /* fragment offset mask */
#define  IP_PREC      0xe0    /* precedence port of TOS */

/* IP Precedence values */

#define IPP_NETCTL  0xe0    /* network control          */
#define IPP_INCTL   0xc0    /* internet control         */
#define IPP_CRIT    0xa0    /* critical             */
#define IPP_FLASHO  0x80    /* flash over-ride          */
#define IPP_FLASH   0x60    /* flash                */
#define IPP_IMMED   0x40    /* immediate                */
#define IPP_PRIO    0x20    /* priority             */
#define IPP_NORMAL  0x00    /* normal               */

/* macro to compute a datagram's header length (in bytes)		*/
//低4位
#define  IP_HLEN(pip)    ((pip->ip_verlen & 0xf) << 2)
#define  IPMHLEN         20   /* minimum IP header length (in bytes) */ 

/* IP options */
#define  IPO_COPY    0x80     /* copy on fragment mask */
#define  IP_CLASS    0x60     /* option class          */
#define  IPO_NUM     0x17     /* option number         */

#define  IPO_EOOP    0x00     /* end of options        */
#define  IPO_NOP     0x01     /* no operation				*/
#define  IPO_LSRCRT  0x82     /* DoD security/compartmentalization */
#define  IPO_SSRCRT  0x89     /* strict source routing      */
#define  IPO_RECRT   0x07     /* record route            */
#define  IPO_STRID   0x88     /* stream ID              */
#define  IPO_TIME    0x44     /* internet timestamp			*/

#define  IP_MAXLEN   (BPMAXB - EP_HLEN)

/* IP process info */
extern int32_t ipproc(void* );
#define IPNAM  "ip"    /* name of ip process */
#define	IPARGC		0	/* count of args to IP 			*/

extern IPaddr	ip_maskall;	/* = 255.255.255.255			*/
extern IPaddr	ip_anyaddr;	/* = 0.0.0.0				*/
extern IPaddr	ip_loopback;	/* = 127.0.0.1				*/

extern	int	ippid, gateway;

#define IPPrint(pre,ip) cprintf("%s : %d.%d.%d.%d\n",pre , (ip&0xff000000)>>24, (ip&0x00ff0000)>>16,(ip&0x0000ff00)>>8,(ip&0x000000ff))

#endif  // ! __KERN_NET_TCPIP_H_IP_H__


