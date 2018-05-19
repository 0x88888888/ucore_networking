#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * arptimer - 定时处理arptable
 *------------------------------------------------------------------------
 *	gran	- time since last iteration,单位:秒
 */
void arptimer(int gran) {
    struct arpentry  *pae;
    int intr_flag;
    int i;
    local_intr_save(intr_flag);
    
    //arptable中的都是主机序
    for (i=0; i< ARP_TSIZE; i++) {
        if ((pae = &arptable[i])->ae_state == AS_FREE) {
        	continue;
        }
        if (pae->ae_ttl == ARP_INF) {
        	//永久有效,跳过
        	continue;
        }

        if ((pae->ae_ttl -= gran) <= 0) {
            //小于0，需要处理expire了
        	if (pae->ae_state == AS_RESOLVED) {
        		pae->ae_state = AS_FREE;
        	} else if (++pae->ae_attempts > ARP_MAXRETRY) {
                //之前处于正在等待状态,已经arp requst过了maxretry次了
        		pae->ae_state = AS_FREE;
        		arpdq(pae);
        	} else {
                arpprint();
                //还没有过了maxretry次，在retry
        		pae->ae_ttl = ARP_RESEND;
				arpsend(pae);
        	}
        }
    }

    local_intr_restore(intr_flag);
}
