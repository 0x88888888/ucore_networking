#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * arpqsend - write packets queued waiting for an ARP resolution
 *            将缓存在ae_queue中的数据发送出去
 *------------------------------------------------------------------------
 */

void arpqsend(struct arpentry* pae) {
	struct ep*      pep;
	struct netif*   pni;

	if (is_emptyq(pae->ae_queue)){
		return;
	}

	pni = pae->ae_pni;
	while( (pep= (struct ep*)deq(pae->ae_queue)) ) {
		
		netwrite(pni, pep, pep->ep_len);
	}

	assert(is_emptyq(pae->ae_queue));
}

