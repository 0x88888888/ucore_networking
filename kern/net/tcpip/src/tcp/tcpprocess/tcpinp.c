#include <tcpip/h/network.h>
/* tcpin 进程，接受来自 ip层的数据 */

int tcps_oport, tcps_iport, tcps_lqsize;
mutex tcps_tmutex;

int32_t
tcpinp(void* args) {
    struct ep   *pep;
    struct ip   *pip;
    struct tcb  *ptcb;

    tcps_iport = pcreate(TCPQLEN);//最多29个消息
    unlock(&Net.sema);
    while(true) {
        //重输入端口读取tcp数据
    	pep = (struct ep*) preceive(tcps_iport);
        cprintf("tcpinp pep = %x \n", pep);
    	if ((int)pep==SYSERR){
    		break;
    	}
    	pip = (struct ip*)pep->ep_data;
        //头检查
        int tcplen = pip->ip_len - IP_HLEN(pip);
        struct tcp* ptcp = (struct tcp*)((char*)pip +  IP_HLEN(pip));
        
        if(tcpcksum2( pip->ip_src, pip->ip_dst, pip->ip_proto ,tcplen ,ptcp)) {          
            ++TcpInErrs;
            freebuf(pep);
            panic("tcpinp checksum error\n");
            continue;
        }
        
        
        //正式运行时，需要注释掉
        //local_intr_save(intr_flag);

        // 转到主机序
    	tcpnet2h(ptcp); 
    	ptcb = tcpdemux(pep);// 得到对应的tcb
    	if (ptcb == 0) {
    		++TcpInErrs;
            //tcprest马上发送 rst
    		tcpreset(pep);
    		freebuf(pep);
    		continue;
    	}

    	if (!tcpok(ptcb, pep)){ 
            /*
             * 1.没有数据传送过来,并且ptcp->tcp_seq不再范围内
             * 2.数据不再窗口范围内
             * 发送对端ack信息，
             */
    		tcpackit(ptcb, pep);
    	} else {
            //options数据，目前只有max segment size
            tcpopts(ptcb, pep);//处理options数据
            //跳到相应的状态函数去处理
            tcpswitch[ptcb->tcb_state](ptcb, pep);
    	}
    	if (ptcb->tcb_state != TCPS_FREE) {
    		unlock(&(ptcb->tcb_mutex));
    	}
    }

    return OK;
}

