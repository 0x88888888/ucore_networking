#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpsync - 
 *            服务器收到对端的syn标记,初始化服务端tcb
 *------------------------------------------------------------------------
 *  tcpmopen
 *  tcplisten
 *      tcpsync
 *
 */
int
tcpsync(struct tcb *ptcb)
{//初始化一个客户端tcb
	ptcb->tcb_state = TCPS_CLOSED;
	ptcb->tcb_type = TCPT_CONNECTION;
    
    //得到initial send sequence
    //顺手将 suna,snext,lwack设置为 tcb_iss
	ptcb->tcb_iss = ptcb->tcb_suna = ptcb->tcb_snext = tcpiss();
	ptcb->tcb_lwack = ptcb->tcb_iss;
    
    //分配发送缓冲区
	ptcb->tcb_sndbuf = (unsigned char*)kmalloc(TCPSBS);
	ptcb->tcb_sbsize = TCPSBS;
	ptcb->tcb_sbstart = ptcb->tcb_sbcount = 0;
	sem_init(&(ptcb->tcb_ssema), 1);
    
    //奉陪接受缓冲区
    ptcb->tcb_rcvbuf = (unsigned char*)kmalloc(TCPRBS);
    ptcb->tcb_rbsize = TCPRBS;
	ptcb->tcb_rbstart = ptcb->tcb_rbcount = 0;
	ptcb->tcb_rsegq = NULL;
	sem_init(&(ptcb->tcb_rsema), 0);
	sem_init(&(ptcb->tcb_ocsem), 0);

	/* 时间*/
	ptcb->tcb_srt = 0;		/* in sec/100	*/
	ptcb->tcb_rtde = 0;		/* in sec/100	*/
	
	//重传时间,10秒
	ptcb->tcb_rexmt = TCPREXMTTIMEOUT ;		/* in sec/100	*/
	ptcb->tcb_rexmtcount = 0;
	ptcb->tcb_keep = 12000;		/* in sec/100	*/
    
    //这是syn，tcpout发送SYN标记，开始连接
	ptcb->tcb_code = TCPF_SYN;
	ptcb->tcb_flags = 0;
	return OK;
}


