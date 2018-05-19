#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpclosewait -  
 *
 *  被动关闭的一端
 *  接受到一个fin，进入CLOSE_WAIT状态
 *  进入CLOSE_WAIT状态的tcb，表示对方已经不再发送数据了，
 *  但是两端的连接还没有断开，不接受对方的数据了，
 *  在CLOSE_WAIT状态的tcb，只要发送一个FIN就进入LAST_ACK状态了
 *------------------------------------------------------------------------
 * 
 */
int
tcpclosewait(struct tcb *ptcb, struct ep *pep)
{
    struct  ip  *pip = (struct ip *)pep->ep_data;
    struct  tcp *ptcp = (struct tcp *)pip->ip_data;

    if (ptcp->tcp_code & TCPF_RST) {
        TcpEstabResets++;
        TcpCurrEstab--;
        return tcpabort(ptcb, TCPE_RESET);
	}
    if (ptcp->tcp_code & TCPF_SYN) {
        TcpEstabResets++;
        TcpCurrEstab--;
        //tcpreset马上发送 rst
        tcpreset(pep);
        return tcpabort(ptcb, TCPE_RESET);
    }
    //回复对方ack，对方进入 FIN_WAIT_2状态
    tcpacked(ptcb, pep);
    tcpswindow(ptcb, pep);
    return OK;
}


