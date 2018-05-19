#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcptimewait -  主动关闭的一端进入2TMSL
 *------------------------------------------------------------------------
 */
int
tcptimewait(struct tcb *ptcb, struct ep *pep) {
	struct ip     *pip = (struct ip *)pep->ep_data;
	struct tcp    *ptcp= (struct tcp*)pip->ip_data;

	if (ptcp->tcp_code & TCPF_RST) {
		//TIME_WAIT状态收到RST标记，就直接回收tcb了
		return tcbdealloc(ptcb);
	} 
	if (ptcp->tcp_code & TCPF_SYN) {
		//TIME_WAIT状态不接受syn
		//tcpreset马上发送 rst
		tcpreset(pep);
		return tcbdealloc(ptcb);
	}

	tcpacked(ptcb, pep);
	tcpdata(ptcb, pep); //处理数据
	/*
	 * 取消旧的删除事件，安排新的删除事件
     * 如果不断TIME_WAIT状态，不断的接受新来的数据，那就不会删除tcb了
     */
	tcpwait(ptcb);  
	return OK;
}
