#include <tcpip/h/network.h>

int tcpuopt(struct tcb *, int, unsigned int);
int tcpstat (struct tcb* ptcb, struct tcpstat *tcps);
/*------------------------------------------------------------------------
 *  tcpcntl - control function for TCP pseudo-devices
 *------------------------------------------------------------------------
 */
int
tcpcntl(struct device *pdev, int func, void *arg, void* arg2) {
	struct    tcb  *ptcb = (struct tcb*)pdev->dvioblk;
	int       rv;
	
	if (ptcb == NULL || ptcb->tcb_state == TCPS_FREE) {
		return SYSERR;
	}

	lock(&(ptcb->tcb_mutex));
	if (ptcb->tcb_state == TCPS_FREE) {
		return SYSERR;
	}
	switch(func) {
        case TCPC_ACCEPT:
             if(ptcb->tcb_type != TCPT_SERVER) {
             	rv = SYSERR;
             	break;
             }
             signal(&(ptcb->tcb_mutex));
             return preceive(ptcb->tcb_listenq);
        case TCPC_LISTENQ:
             rv= tcplq(ptcb, (int)arg);
             break;
        case TCPC_STATUS:
             rv = tcpstat(ptcb, (struct tcpstat*)arg);
             break;
        case TCPC_SOPT:
        case TCPC_COPT:
             // 选项数据处理,通过ptcb->tcb_flags设置
             rv = tcpuopt(ptcb, func, (unsigned int)arg);
             break;
        case TCPC_SENDURG:
             /*
              * tcpwr acquire and releases tcb_mutex itself.
              */
             unlock(&(ptcb->tcb_mutex));
             return tcpwr(pdev, (unsigned char*)arg, (unsigned int)arg2, TWF_URGENT);
             break;
        default:
             rv=SYSERR;
	}
	unlock(&(ptcb->tcb_mutex));
    return rv;
}

