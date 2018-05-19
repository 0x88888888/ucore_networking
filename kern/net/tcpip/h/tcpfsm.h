#ifndef __KERN_NET_TCPIP_H_TCPFSM_H__
#define __KERN_NET_TCPIP_H_TCPFSM_H__

/* TCP states */

#define TCPS_FREE         0
/*CLOSED表示tcb已经被分配，但是还没有开始使用，处于打开过程中 */
#define TCPS_CLOSED       1
#define TCPS_LISTEN       2
#define TCPS_SYNSENT      3
#define TCPS_SYNRCVD      4
#define TCPS_ESTABLISHED  5
#define TCPS_FINWAIT1     6
#define TCPS_FINWAIT2     7
#define TCPS_CLOSEWAIT    8
#define TCPS_LASTACK      9
#define TCPS_CLOSING      10
#define TCPS_TIMEWAIT     11

#define NTCPSTATES        12


/* Output States */

#define TCPO_IDLE         0
#define TCPO_PERSIST      1
#define TCPO_XMIT         2
#define TCPO_REXMT        3

#define	NTCPOSTATES       4

/* event processing */

#define SEND                0x1
#define PERSIST             0x2
#define RETRANSMIT          0x3
#define DELETE              0x4
#define TMASK               0x7

/* tcb|event */
//得到event
#define EVENT(x)            ((int)(x) & TMASK)
//得到MKEVENT中的tcb序号
#define	TCB(x)		        ((int)(x) >> 3)
//tcb为tcb在tcbtab中的序号
#define	MKEVENT(timer, tcb)	(void *)(((tcb)<<3) | ((timer) & TMASK))

/* implementation parameters */

#define TCP_MAXRETRIES       12        /* max retransmissions before giving up */
#define TCP_TWOMSL           12000     /* 2 minutes (2 * Max Segment Lifetime)	*/
#define TCP_MAXRXT           2000      /* 20 seconds max rexmt time		*/
#define TCP_MINRXT           50        /* 1/2 second min rexmt time		*/
#define TCP_ACKDELAY         20        /* 1/5 sec ACK delay, if TCBF_DELACK	*/

#define TCP_MAXPRS           6000      /* 1 minute max persist time		*/

/* second argument to tcpsend(): */

#define TSF_NEWDATA          0         /* send all new data */
#define TSF_REXMT            1         /* retransmit the first pending segment	*/

/* third argument to tcpwr():	*/

#define	TWF_NORMAL	0	/* normal data write    */
#define	TWF_URGENT	1	/* urgent data write    */

#endif // ! __KERN_NET_TCPIP_H_TCPFSM_H__


