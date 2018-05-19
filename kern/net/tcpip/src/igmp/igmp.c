#include <tcpip/h/network.h>

//IPaddr  ig_allhosts = {0x01,0x00,0x00,0xE0}; 		/* 224.0.0.1 */
//IPaddr  ig_allDmask = {0x00,0x00,0x00,0xF0};		/* 240.0.0.0 */

IPaddr	ig_allhosts = 0xE0000001; 	/* 224.0.0.1 */
IPaddr  ig_allDmask = 0xF0000000;	/* 240.0.0.0 */

/*------------------------------------------------------------------------
 *  igmp  -  send IGMP requests/responses
 *------------------------------------------------------------------------
 */
int igmp(int typ, unsigned int ifnum, IPaddr hga) {
	struct ep*  pep;
	struct ip*  pip;
	struct igmp *pig;

	pep = (struct ep*)kmalloc(sizeof(struct ep));
	if (pep == NULL) {
		return SYSERR;
	}
	pip = (struct ip*)pep->ep_data;
	pig = (struct igmp*)pip->ip_data;
	pig->ig_vertyp = (IG_VERSION<<4) | (typ & 0xf);
	pig->ig_unused = 0;
	pig->ig_cksum = 0;
	pig->ig_gaddr = hga;
	//blkcopy(pig->ig_gaddr, hga, IP_ALEN);
	pig->ig_cksum = cksum((unsigned short*)pig, IG_HLEN>>1);
    
    //panic("igmp not implement fully\n");
	ipsend(hga, pep, IG_HLEN, IPT_IGMP, IPP_INCTL, 1);
	return OK;
}

