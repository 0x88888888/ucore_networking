#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tcpdemux -  do TCP port demultiplexing 
 *              为pep数据匹配到相应的tcb
 *------------------------------------------------------------------------
 *
 * tcpinp
 *   tcpdemux
 */
struct tcb*
tcpdemux(struct ep *pep) {
	struct ip    *pip  = (struct ip*)pep->ep_data;
	struct tcp   *ptcp = (struct tcp*)pip->ip_data;
	int    tcbn, lstcbn;
	
	lock(&tcps_tmutex);
	for (tcbn=0,lstcbn =-1; tcbn< Ntcp; tcbn++) {
		if (tcbtab[tcbn].tcb_state == TCPS_FREE) {
			continue;
		}
		if (ptcp->tcp_dport == tcbtab[tcbn].tcb_lport &&
			ptcp->tcp_sport == tcbtab[tcbn].tcb_rport &&
			pip->ip_src == tcbtab[tcbn].tcb_rip &&
			pip->ip_dst == tcbtab[tcbn].tcb_lip) {
			//完全匹配
			break;
		}
		//listen的那个tcb
		if (tcbtab[tcbn].tcb_state == TCPS_LISTEN &&
			ptcp->tcp_dport == tcbtab[tcbn].tcb_lport) {
			lstcbn = tcbn;
		}
	}
	if (tcbn >= Ntcp) {
		//首次syn过来
		if(ptcp->tcp_code & TCPF_SYN) {
			tcbn = lstcbn;
		} else {
			tcbn = -1;
		}
	}
    unlock(&tcps_tmutex);
    if (tcbn < 0) {
    	return 0;
    }
    lock(&tcbtab[tcbn].tcb_mutex);
    if (tcbtab[tcbn].tcb_state == TCPS_FREE) {
    	return 0;   /* OOPS! Lost it */
    }
    return &tcbtab[tcbn];
}




