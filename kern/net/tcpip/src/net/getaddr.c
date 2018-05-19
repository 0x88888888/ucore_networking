#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  getiaddr  -  obtain this interface's complete address (IP address)
 *------------------------------------------------------------------------
 */
int32_t getiaddr(int32_t inum) {
	panic("getiaddr doesn't be implemented\n");

	struct netif *pif;
	if (inum == NI_LOCAL || inum < 0 || inum >= Net.nif) {
		return SYSERR;
	}

	pif = &nif[inum];
	lock(&rarpsem);
	if(!pif->ni_ivalid && pif->ni_state == NIS_UP) {
		rarpsend(inum);
	}
	unlock(&rarpsem);

	if (!pif->ni_ivalid) {
		return SYSERR;
	}
	//blkcopy(ip, intf->ni_ip, IP_ALEN);
	return pif->ni_ip;
}

/*------------------------------------------------------------------------
 *
 *  getaddr - obtain this hosts's primary IP address (xyz)
 *
 *------------------------------------------------------------------------
 */
int32_t getaddr(int inum) {

	return getiaddr(NI_PRIMARY);
}

