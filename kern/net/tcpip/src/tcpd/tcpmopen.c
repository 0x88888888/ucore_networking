#include <tcpip/h/network.h>

int tcpserver(struct tcb *, unsigned short);
int tcpbind(struct tcb *, char *, unsigned short);
int tcpcon(struct tcb *);

/*------------------------------------------------------------------------
 *  tcpmopen  -  从devtab中分配一个设备
 *------------------------------------------------------------------------
 */
int
tcpmopen(struct device *pdev, void *fport0, void *lport0) {
	char    *fport = (char*)fport0;
	int     lport = (int)lport0;
	struct  tcb  *ptcb;
	int     error;

	ptcb = (struct tcb*)tcballoc();
	if (ptcb == (struct tcb*)SYSERR) {
		return SYSERR;
	}
	ptcb->tcb_error = 0;
	current->ptcpumode =false;
	if (fport == ANYFPORT) {
		//作为服务端打开
		return tcpserver(ptcb, lport);
	}

    //作为客户端打开，绑定，发送syn标记
	if (tcpbind(ptcb, fport, lport)!= OK ||
		tcpsync(ptcb) != OK) {
        //绑定或者syn出错
		ptcb->tcb_state = TCPS_FREE;
	    unlock(&(ptcb->tcb_mutex));
	    //sdelete(&(ptcb->tcb_mutex));
	    return SYSERR;
	}
	
	if( (error = tcpcon(ptcb)) ) {
		return error;
	}
	return ptcb->tcb_dvnum;
}

