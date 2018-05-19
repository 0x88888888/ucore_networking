#ifndef __KERN_NET_TCPIP_H_TCB_H__
#define __KERN_NET_TCPIP_H_TCB_H__
#include <tcpip/h/tcb.h>
/* TCP endpoint types */
#define TCPT_SERVER          1
#define TCPT_CONNECTION      2
#define TCPT_MASTER          3

/* TCP process info */
extern  int32_t   tcpip();
#define	TCPINAM	  "tcpinp"	/* name of TCP input process	*/

extern  int32_t   tcpout();
#define	TCPONAM	  "tcpout"  /* name of TCP output process	*/

#define TCPQLEN    20       /* TCP process port queue length	*/

#define TCPMAXURG  MAXNETBUF  /* maximum urgent data buffer size	*/

/* TCP exceptional conditions */
/*连接重置*/
#define TCPE_RESET       -1
/*请求被拒绝*/
#define TCPE_REFUSED     -2
/*报文段过大*/
#define TCPE_TOOBIG      -3
/*超时*/
#define TCPE_TIMEDOUT    -4
/*紧急数据读取模式下无法接受普通数据*/
#define TCPE_URGENTMODE  -5
/*普通模式下无法接受紧急数据*/
#define TCPE_NORMALMODE  -6

/* string equivalents of TCPE_*, in "tcpswitch.c" */
extern char    *tcperror[];

#define READERS          1
#define WRITERS          2

/* tcb_flags */
/*需要发送数据*/
#define TCBF_NEEDOUT     0x01   /* we need output */
/*首次发送报文段，不需要ack置位*/
#define TCBF_FIRSTSEND   0x02   /* no data to ack */
/* 收到一个FIN标记的报文段 */
#define TCBF_GOTFIN      0x04   /* no more to receive */
/* TCBF_RDONE表示接受到一个fin标记或者abort 本地的连接对象了 */
#define TCBF_RDONE       0x08   /* no more receive data to process */
/* 只有在tcpabort中设置 TCBF_SDONE,表示 abort本地连接对象 */
#define TCBF_SDONE       0x10   /* no more send data allowed       */
/*延迟确认*/
#define TCBF_DELACK      0x20   /* do delayed ACK's                */
/*启动非阻塞读取模式*/
#define TCBF_BUFFER      0x40   /* do TCP buffering (default to)   */
/* 在tcpdodat中标记, 只有 TCPF_PSH或者 TCPF_URG就标记TCBF_PUSH*/
#define TCBF_PUSH        0x80   /* got a push; deliver what we have*/
/*发送一个FIN标记*/
#define TCBF_SNDFIN      0x100  /* user process has closed; send a FIN */
//有urgent数据接受进来
#define TCBF_RUPOK       0x200  /* receive urgent pointer is valid */
//有urgent数据需要发送,在tcpwr,tcpacked中标记
#define TCBF_SUPOK       0x400  /* send urgent pointer is valid    */

/* aliases, for user programs */
 
#define TCP_BUFFER    TCBF_BUFFER
#define TCP_DELACK    TCBF_DELACK

/* receive segment reassembly data */

#define NTCPFRAG      10

struct tcb {
	short           tcb_state;      //当前连接的输入状态机
	short           tcb_ostate;     //当前连接的输出状态机
	/*
	 *tcb_type 记录当前连接是在哪一端，TCPT_SERVER表示在服务端
	 *                                 TCPT_CONNECTION 表示在客户端
	 */
	short           tcb_type;       /* TCP type (SERVER,CLIENT) */

	mutex           tcb_mutex;      /* tcb mutual exclusion */
	/*对应 ptcp->tcp_code*/
    
    //TCPF_XXXX
	short           tcb_code;       /* TCP code for next packet */
	
    //TCBF_XXXX
    short           tcb_flags;      /* various TCB state flags  */
	short           tcb_error;      /* return error for user side */
 
	IPaddr          tcb_rip;        /* 对端ip地址 */
	unsigned short  tcb_rport;      /* 对端tcp端口 */
	IPaddr          tcb_lip;        /* 本端ip地址   */
	unsigned short  tcb_lport;      /* 本端tcp端口 */
	struct netif    *tcb_pni;       
    
    //发送出去，但是没有被ack的最低序列号
	tcpseq          tcb_suna;       /* send unacked */
	//准备发送的下一个报文的序号 ptcp->tcp_seq
	tcpseq          tcb_snext;      /* send next  */
	//如果发送了FIN，就记录那个报文的序号 tcp_seq
	tcpseq          tcb_slast;      /* sequence of FIN if TCBF_SNDFIN */
    /*
     * 记录对方的tcp_window 
     * tcpwinit, tcpsynsent,tcpswindow 中设置
     *
     */
	unsigned long   tcb_swindow;    /* send window size (octets) */
	//记录对方发过来的tcp_seq
	tcpseq          tcb_lwseq;      /* sequence of last window update */
	//记录对方发过来的tcp_ack
	tcpseq          tcb_lwack;      /* ack seq of last window update */
	

    //拥塞窗口值
	unsigned int    tcb_cwnd;       /* congestion window size (octets)注意:这个tcb_cwnd不是 ptcp->tcp_window */
	//慢启动阀值
	unsigned int    tcb_ssthresh;   /* 慢开始门限 slow start threshhold (octets) */
	//对端的max segement size，不包括tcp头，但是包括option
	unsigned int    tcb_smss;  
	//初始 ptcp->tcp_seq
	tcpseq          tcb_iss;    
    //平滑往返时延估值
	int             tcb_srt;        /* smoothed Round Trip Time       */
	//往返时延偏差估值
	int             tcb_rtde;       /* Round Trip deviation estimator    */
	
    
    //tcp处在persist状态时的保持时间
	int             tcb_persist;    /* persist timeout value          */
	//连接保活定时器的保持值
	int             tcb_keep;       /* keepalive timeout value */
	//超时重传时间 RTO
    int             tcb_rexmt;      /* retransmit timeout value  */
    //重传次数
    int             tcb_rexmtcount; /* number of remxts sent */
    //希望接受的下一个序号。
    //在发送tcp数据包的时候，用于填写 ptcp->tpc_ack
    tcpseq          tcb_rnext;      /* receive next            */

    tcpseq          tcb_rupseq;     /* receive urgent pointer  */
    tcpseq          tcb_supseq;     /* send urgent pointer     */
    //监听队列的长度
    int             tcb_lqsize;     /* listen queue size (SERVERs) */
    //监听队列使用的内部端口
    int             tcb_listenq;    /* listen queue port(Servers) */
    //父tcb，listen的那个tcb
    struct tcb      *tcb_pptcb;     /* pointer to parent TCB (for accept) */
    //保证连接的打开/关闭操作
    semaphore_t     tcb_ocsem;      /* open/close semaphore */
    /* devtab中索引 */
    int             tcb_dvnum;      /* TCP slave psuudo device number */
    

    //保证发送缓冲区访问互斥
    semaphore_t     tcb_ssema;      /* send semaphore  */
    //tcp发送缓冲区
    unsigned char   *tcb_sndbuf;    /* send buffer */
    //发送缓冲区的有效数据的起始地址，环形
    unsigned int    tcb_sbstart;    /* start of valid data */
    //发送缓冲区的有效数据长度，发送缓冲区中的字符数量
    unsigned int    tcb_sbcount;    /* data character count */
    //发送缓冲区的大小,总空间
    unsigned int    tcb_sbsize;     /* send buffer size (bytes) */
    


    //保证接收缓冲区访问的互斥
    semaphore_t   tcb_rsema;      /* receive semaphore */
    //tcp接收缓冲区
    unsigned char *tcb_rcvbuf;    /* receive buffer (circular)		*/
    //接收缓冲区的有效数据的起始地址
    unsigned int  tcb_rbstart;    /* start of valid data */
    //接收缓冲区的有效数据长度，可用空间
    unsigned int  tcb_rbcount;    /* data character count */
    //接收缓冲区的大小,总空间
    unsigned int  tcb_rbsize;     /* receive buffer size (bytes) */


    //接收方最大报文长度
    unsigned int  tcb_rmss;       /* receive max segment size */
    //前一次通告窗口,本端当前窗口可接收的最大报文序号, (ptcp->tcp_window)的序号
    tcpseq        tcb_cwin;       /* seq of currently advertised window  */
    //报文段分片队列序号
    queue*        tcb_rsegq;      /* segment fragment queue */
    //收到的fin报文段末尾序号，如没有收到就置为0
    tcpseq        tcb_finseq;     /* FIN sequence number or 0 */
    //收到的push报文段序号，如没有收到就置为0
    tcpseq        tcb_pushseq;    /* PUSH sequence number of 0 */
};

/* TCP fragment structure */

struct tcpfrag {
	tcpseq  tf_seq;
	int     tf_len;
};

/* TCP control() functions */
#define TCPC_LISTENQ     0x01    /* set the listen queue length */
#define TCPC_ACCEPT      0x02    /* wait for connect after passive open */
#define TCPC_STATUS      0x03    /* return status info (all, for master) */
#define TCPC_SOPT        0x04    /* set user-selectable options */
#define TCPC_COPT        0x05    /* clean user-selectable options */
#define TCPC_SENDURG     0x06    /* write urgent data */

/* global state information */

extern int tcps_oport;
extern int tcps_iport;
extern int tcps_lqsize;     // default SERVER queue size
extern mutex tcps_tmutex;  /* tcb table mutex */

extern int (*tcpswitch[])(struct tcb*, struct ep*),
        (*tcposwitch[])(int ,int);

#ifdef Ntcp 
extern struct tcb tcbtab[];
#endif        

struct tcb *tcpdemux(struct ep *), *tcballoc(void);
int tfcoalesce(struct tcb *, unsigned int , struct tcp *);
int tfinsert(struct tcb *, tcpseq, unsigned int);
int tcbdealloc(struct tcb *);
bool tcpok(struct tcb *, struct ep *);
int tcpackit(struct tcb *, struct ep *);
int tcpwait(struct tcb *);
int tcpdata(struct tcb *, struct ep *);
int tcpacked(struct tcb *, struct ep *);
int tcpabort(struct tcb *, int);
int tcpopts(struct tcb *, struct ep *);
int tcpswindow(struct tcb *, struct ep *);
int tcpdodat(struct tcb *, struct tcp *, tcpseq, unsigned int);
int tcpkick(struct tcb *);
int tcpwakeup(int, struct tcb *);
int tcpostate(struct tcb *, int);
int tcprwindow(struct tcb *);
int tcprtt(struct tcb *);
int tcpsync(struct tcb *);
int tcpwinit(struct tcb *, struct tcb *, struct ep *);
int tcpsmss(struct tcb *, struct tcp *, unsigned char *);
int tcprmss(struct tcb *, struct ip *);
int tcpsndlen(struct tcb *, bool, unsigned int *);
int tcphowmuch(struct tcb *);
int tcplq(struct tcb *, unsigned int);


#endif  //!__KERN_NET_TCPIP_H_TCB_H__


