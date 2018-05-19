#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpout - handle events affecting TCP output processing
 *------------------------------------------------------------------------
 * tcp 输出进程
 */
int32_t tcpout(void* args) {
	struct tcb   *ptcb;
	int   i;

    tcps_oport = pcreate(TCPQLEN);
    unlock(&Net.sema); 

    while(true) {
    	i = preceive(tcps_oport);
    	ptcb = &tcbtab[TCB(i)];
    	if (ptcb->tcb_state <= TCPS_CLOSED) {
           continue;        /* a rogue; ignore it */
    	}
    	lock(&(ptcb->tcb_mutex));
    	if (ptcb->tcb_state <= TCPS_CLOSED) {
    		continue;       /* TCB deallocated */
    	}
    	if (EVENT(i)== DELETE) { /* same for all states	*/
            tcbdealloc(ptcb);
    	} else {
            //输出数据
            /* 4个状态函数
             * tcpidle  ->如果是SEND事件，这个会调用 tcpxmit
             * tcppersist
             * tcpxmit
             * tcprexmt
             */
            //panic("tcpout panic\n");
            switch(ptcb->tcb_ostate) {
                 case TCPO_IDLE:
                      cprintf("\n-----------------------------\n");
                      cprintf("TCPO_IDLE EVENT= %x\n",EVENT(i));

                      cprintf("\n-----------------------------\n");
                      break;
                 case TCPO_PERSIST:
                      cprintf("\n-----------------------------\n");
                      cprintf("TCPO_PERSIST EVENT= %x\n",EVENT(i));
                      cprintf("\n-----------------------------\n");
                      break;
                 case TCPO_XMIT:
                      cprintf("\n-----------------------------\n");
                      cprintf("TCPO_XMIT EVENT= %x\n",EVENT(i));
                      cprintf("\n-----------------------------\n");
                      break;
                 case TCPO_REXMT:
                      cprintf("\n-----------------------------\n");
                      cprintf("TCPO_REXMT EVENT= %x\n",EVENT(i));
                      cprintf("\n-----------------------------\n");
                      break;
                 default :
                      panic("tcpout error\n");
            }
            
    		tcposwitch[ptcb->tcb_ostate](TCB(i), EVENT(i));
    	}

    	if (ptcb->tcb_state != TCPS_FREE) {
    		unlock(&(ptcb->tcb_mutex));
    	}
    }   
    return OK;
}





