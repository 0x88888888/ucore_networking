#include <tcpip/h/network.h>

/*
 * tcp_in - 处理收到的tcp包
 * 
 * ipproc
 * local_out
 *     tcp_in
 */
int
tcp_in(struct netif *pni, struct ep *pep) {
    
    /* drop instead of blocking on psend */
    TcpInSegs++;
    if (pcount(tcps_iport) >= TCPQLEN) {
    	freebuf(pep);
    	return SYSERR;
    }    
    
    //将接受过来的数据发送到 tcpin_process
    cprintf("tcp_in, tcps_iport= %d\n", tcps_iport);
    psend(tcps_iport, (int)pep);
    return OK;
}


