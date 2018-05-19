#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpcon - initiate a connection
 *  发起连接
 *------------------------------------------------------------------------
 *  tcpmopen
 *     tcpcon
 *  
 */
int
tcpcon(struct tcb *ptcb) {
	//struct netif  *pni = ptcb->tcb_pni;
	struct route  *prt;
	bool          local;
	int    error, mss;

	prt = (struct route *)rtget(ptcb->tcb_rip, RTF_REMOTE);
	local = prt && prt->rt_metric == 0;
	rtfree(prt);
	if (local){
		mss = ptcb->tcb_pni->ni_mtu-IPMHLEN-TCPMHLEN;
	}else{
		mss = 536;			/* RFC 1122 		*/
	}

    ptcb->tcb_smss = mss;   /* default */
    ptcb->tcb_rmss = ptcb->tcb_smss;  
    ptcb->tcb_swindow = ptcb->tcb_smss; /* conservative		*/
    ptcb->tcb_cwnd = ptcb->tcb_smss;    /* 1 segment */
    ptcb->tcb_ssthresh = 65535;        /* IP Max window size	*/
    ptcb->tcb_rnext = 0;
    ptcb->tcb_finseq = 0;
    ptcb->tcb_pushseq = 0;
    ptcb->tcb_flags = TCBF_NEEDOUT | TCBF_FIRSTSEND;
    ptcb->tcb_ostate = TCPO_IDLE;
    ptcb->tcb_state = TCPS_SYNSENT;
    tcpkick(ptcb);
    ptcb->tcb_listenq = SYSERR;

    TcpActiveOpens++;
    lock(&(ptcb->tcb_mutex));
    wait(&(ptcb->tcb_ocsem));
    if ( (error = ptcb->tcb_error)) {
    	tcbdealloc(ptcb);
    }
    return error;
}


