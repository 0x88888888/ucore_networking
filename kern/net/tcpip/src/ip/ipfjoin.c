#include <tcpip/h/network.h>

struct ep* ipfjoin(struct ipfq* iq) {
	struct ep*  pep;
	struct ip*  pip = 0;
	int off, packoff;

	if (iq->ipf_state == IPFF_BOGUS) {
		return 0;
	}
    /* see if we have the whole datagram */

    off = 0;

    while((pep = (struct ep*)(seeq(iq->ipf_q))  )) {
    	packoff = (pip->ip_fragoff & IP_FRAGOFF) << 3;
    	if (off < packoff) {
    		while( (seeq(iq->ipf_q)) ) {
    			return 0;
    		}
    	}
    	off = packoff + pip->ip_len - IP_HLEN(pip);
    }
    if (off > MAXLRGBUF) {		/* too big for us to handle */
        while ( (pep = ((struct ep *)(deq(iq->ipf_q)))) ){
            freebuf(pep);
        }
        
        assert(is_emptyq(iq->ipf_q));
        iq->ipf_state = IPFF_FREE;
        return 0;
	}
    if ((pip->ip_fragoff & IP_MF) == 0)
        return ipfcons(iq);

    return 0;
}



