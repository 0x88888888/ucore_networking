#ifndef __KERN_NET_TCPIP_H_TCP_H__ 
#define __KERN_NET_TCPIP_H_TCP_H__

/*
 * SEQCMP - sequence space comparator
 *	This handles sequence space wrap-around. Overlow/Underflow makes
 * the result below correct ( -, 0, + ) for any a, b in the sequence
 * space. Results:	result	implies
 *			  - 	 a < b
 *			  0 	 a = b
 *			  + 	 a > b
 */
#define	SEQCMP(a, b)	((a) - (b))

/* tcp packet format */

struct tcp {
  /* 2 */  unsigned short  tcp_sport; 
  /* 2 */  unsigned short  tcp_dport;  
  /* 4 */          tcpseq  tcp_seq;
  /* 4 */          tcpseq  tcp_ack;
  /* 1 */  unsigned  char  tcp_offset; /* 第4位为TCP头部长度，除去选项数据,数据的开始位置，只用前4位 15*4字节，tcp头最长限制在60字节 */
  /* 1 */  unsigned  char  tcp_code;   /* tcp flags */
  /* 2 */  unsigned short  tcp_window;
  /* 2 */  unsigned short  tcp_cksum;
  /* 2 */  unsigned short  tcp_urgptr;/* urgent data position */
           unsigned char   tcp_data[1];
};

struct pseudo_tcp_hdr {
   uint32_t src_ip;
   uint32_t dst_ip;
   uint8_t reserved;
   uint8_t protocol;
   uint16_t len;
} __attribute__((__packed__));

/* TCP Control Bits */

#define TCPF_URG    0x20
#define TCPF_ACK    0x10
#define TCPF_PSH    0x08
#define TCPF_RST    0x04
#define TCPF_SYN    0x02
#define TCPF_FIN    0x01

#define TCPMHLEN    20      /* minimum tcp header length */
#define TCPHOFFSET  0x50    /* tcp_offset value for TCPMHLEN */
#define TCP_HLEN(ptcp)   (((ptcp)->tcp_offset & 0xf0)>>2)

/* TCP Options */

#define	TPO_EOOL	0	/* end Of Option List			*/
#define	TPO_NOOP	1	/* no Operation				*/
#define	TPO_MSS		2	/* maximum Segment Size			*/

/*
 tcp options
 0 end of options list
 1 No operation (Nop, Pad)
 2 Maximum segment size
 3 Window Scale
 4 Selective ACK ok
 5 TimeStamp
*/

int tcpxmit(int, int);
struct tcp *tcpnet2h(struct tcp *), *tcph2net(struct tcp *);
int tcpreset(struct ep *);
int tcpsend(int, bool);
int tcpiss(void);
int tcprexmt(int, int);
int tcpwr(struct device *, unsigned char *, unsigned int, bool);

#endif  //!__KERN_NET_TCPIP_H_TCP_H__


