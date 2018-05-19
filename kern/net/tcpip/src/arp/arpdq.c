#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * arpdq - destroy an arp queue that has expired
 *------------------------------------------------------------------------
 */
void arpdq(struct arpentry *pae) {
	struct ep*  pep;
	struct ip*  pip;

	if ( pae->ae_queue ==NULL || is_emptyq(pae->ae_queue ) ) {
		return ;
	}

	while( (pep = (struct ep*)deq(pae->ae_queue)) ) {
        if(gateway && pae->ae_prtype == EPT_IP) {
        	//如果本机是网关(路由器)，就要返回icmp目的不可达错误
        	pip = (struct ip*)pep->ep_data;
        	icmp(ICT_DESTUR, ICC_HOSTUR, pip->ip_src, pep, 0);
        } else {
        	freebuf(pep);
        }
	}

	//freeq(pae->ae_queue);  // useless ,not be implemented
    assert(is_emptyq(pae->ae_queue));

}

 