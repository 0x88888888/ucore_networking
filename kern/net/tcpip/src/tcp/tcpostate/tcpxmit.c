#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpxmit - handle TCP output events while we are transmitting
 *------------------------------------------------------------------------
 *
 *  tcpidle
 *     tcpxmit
 *  
 *  tcpout
 *     tcpxmit
 *
 *
 */
int32_t tcpxmit(int tcbnum, int event) {
     
     struct   tcb   *ptcb  = &tcbtab[tcbnum];
     void     *tv;
     int      tosend, pending, window;

     if (event == RETRANSMIT) {
     	tmclear(tcps_oport, MKEVENT(SEND, tcbnum));
     	tcprexmt(tcbnum, event);
     	ptcb->tcb_ostate = TCPO_REXMT;
     	return OK;
     } /* else SEND */
     //要发送的数据量
     tosend = tcphowmuch(ptcb);
     if (tosend == 0) {
          //没有数据
     	if (ptcb->tcb_flags & TCBF_NEEDOUT){
     		tcpsend(tcbnum, TSF_NEWDATA);   /* just an ACK */
     	}
     	if (ptcb->tcb_snext == ptcb->tcb_suna) {
     		return OK;
     	}
     	/* still unacked data; restart transmit timer */
          //发送结束后，创建一个定时器，重传
                    //TCP事件      tcb编号
     	tv = MKEVENT(RETRANSMIT, tcbnum);
     	if (!tmleft(tcps_oport, tv)){
     		tmset(tcps_oport, TCPQLEN, tv, ptcb->tcb_rexmt);
     	}
     	return OK;
     } else if (ptcb->tcb_swindow == 0){
          //对方通告的串口大小为0
     	ptcb->tcb_ostate = TCPO_PERSIST;
     	ptcb->tcb_persist = ptcb->tcb_rexmt;
     	tcpsend(tcbnum, TSF_NEWDATA);
     	tmset(tcps_oport, TCPQLEN, MKEVENT(PERSIST, tcbnum),
     		   ptcb->tcb_persist);
     	return OK;
     }
     //有数据可以发送

     ptcb->tcb_ostate = TCPO_XMIT;
     //在通告窗口和拥塞窗口中取小的，以免网络堵
     window = min(ptcb->tcb_swindow, ptcb->tcb_cwnd);

     //pending为没有被ack的数据量
     pending = ptcb->tcb_snext - ptcb->tcb_suna;
     while(tcphowmuch(ptcb) > 0 && pending < window) {
          //要发送的数据量在 对端的通告窗口内，并且也在 网络拥塞窗口内
     	tcpsend(tcbnum, TSF_NEWDATA);
          // tcpsend中会更新tcb_snext序号,
          // 循环到 pending一直到window为止
     	pending = ptcb->tcb_snext - ptcb->tcb_suna;
     }

     
     //设置一个重发事件
     tv = MKEVENT(RETRANSMIT, tcbnum);
     if (!tmleft(tcps_oport, tv)) {
     	tmset(tcps_oport, TCPQLEN, tv, ptcb->tcb_rexmt);
     }
     return OK;
}

