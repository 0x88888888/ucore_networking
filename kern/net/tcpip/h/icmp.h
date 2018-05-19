#ifndef __KERN_NET_TCPIP_H_ICMP_H__
#define __KERN_NET_TCPIP_H_ICMP_H__
/* Internet Control Message Protocol Constants and Packet Format */

/*ic_type field*/
#define ICT_ECHORP     0   /* Echo reply */
#define ICT_DESTUR     3   /* Destination unreachable */
#define ICT_SRCQ       4   /* Source quench */
#define ICT_REDIRECT   5   /* Redirect message type */
#define ICT_ECHORQ     8   /* Echo request */
#define ICT_TIMEX      11  /* Time exceed */
#define ICT_PARAMP     12  /* parameter problem */
#define ICT_TIMERQ     13  /* timestamp request */
#define ICT_TIMERP     14  /* timestamp reply  */
#define ICT_INFORQ     15  /* information request */
#define ICT_INFORP     16  /* information reply   */
#define ICT_MASKRQ     17  /* mask request */
#define ICT_MASKRP     18  /* mask reply        */

/* ic_code field */ 
#define ICC_NETUR      0   /* dest unreachable, net unreachable */
#define ICC_HOSTUR     1   /* dest unreachable, host unreachable */
#define ICC_PROTOUR    2   /* dest unreachable, proto unreachable */
#define ICC_PORTUR     3   /* dest unreachable, port unreachable */
#define ICC_FNADF      4   /* dest unreachable, frag needed & don't frag */
#define ICC_SRCRT      5   /* dest unreachable, src route failed */

#define ICC_NETRD      0   /* redirect:net        */
#define ICC_HOSTRD     1   /* redirect:host       */
#define ICC_TOSNRD     2   /* redirect: type of service,net */
#define ICC_TOSHRD     3   /* redirect: type of service,host */

#define ICC_TIMEX      0   /* time exceed,ttl    */
#define ICC_FTIMEX     1   /* time exceed, frag  */

#define IC_HLEN        8   /* icmp head length				*/
#define IC_PADLEN      3   /* pad length    			*/

#define IC_RDTTL       300 /* ttl for redirect routes */

/* ICMP packet format (following the IP header)				*/

struct icmp   {   /*  ICMP packet   */
    char  ic_type;    /* type of message (ICT_* above) */
    char  ic_code;    /* code (ICC_* above) */   
    short ic_cksum;   /* checksum  of ICMP header+data */

    union {
    	struct {
            //ic1,echo的请求和问答,时间戳的请求和问答，地址掩码的请求和问答
    		short   ic1_id;    /* for echo type, a message id */
    		short   ic1_seq;   /* for echo type, a seq. number */
    	} ic1;
        //返回 icmp redirect
    	IPaddr   ic2_gw;        
    	struct {
            //参数问题报文的首部后4个字节
            char    ic3_ptr;     /* pointer, for ICT_PARAMP */
            char	ic3_pad[IC_PADLEN];
    	} ic3;
        // 终点不可达，源点抑制 超时报文
        long ic4_mbz;    /* must be zero         */
    } icu;
    char   ic_data[1];		/* data area of ICMP message	*/
};

/* format 1 */
#define	ic_id     icu.ic1.ic1_id
#define	ic_seq    icu.ic1.ic1_seq

/* format 2 */
#define	ic_gw     icu.ic2_gw

/* format 3 */
#define	ic_ptr    icu.ic3.ic3_ptr
#define	ic_pad    icu.ic3.ic3_pad

/* format 4 */
#define	ic_mbz    icu.ic4_mbz

#endif //!__KERN_NET_TCPIP_H_ICMP_H__

