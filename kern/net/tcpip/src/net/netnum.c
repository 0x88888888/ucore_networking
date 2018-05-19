#include <tcpip/h/network.h>

IPaddr	ip_maskall = 0xffffffff;//{ 255, 255, 255, 255 };

/*------------------------------------------------------------------------
 *  netnum  - 得到ipa的网络部分到net
 *------------------------------------------------------------------------
 */
IPaddr netnum(IPaddr ipa) {
	IPaddr	mask = ~0;

	if (IP_CLASSA(ipa)) mask = htonl(0xff000000);
	if (IP_CLASSB(ipa)) mask = htonl(0xffff0000);
	if (IP_CLASSC(ipa)) mask = htonl(0xffffff00);
	return ipa & mask;
}

/*------------------------------------------------------------------------
 *  netmask  -  用net计算出网络掩码mask
 *------------------------------------------------------------------------
 */
IPaddr netmask(IPaddr net) {
	IPaddr	netpart;
	int	i;

	if (net == 0)
		return net;
	/* check for net match (for subnets) */

	netpart = netnum(net);
	for (i=0; i<Net.nif; ++i)
		if (nif[i].ni_svalid && nif[i].ni_ivalid &&
		    nif[i].ni_net == netpart)
			return nif[i].ni_mask;
	if (IP_CLASSA(net)) return htonl(0xff000000);
	if (IP_CLASSB(net)) return htonl(0xffff0000);
	if (IP_CLASSC(net)) return htonl(0xffffff00);
	return ~0;
}

extern int bsdbrc;		/* use Berkeley (all-0's) broadcast	*/

/*------------------------------------------------------------------------
 *  setmask - set the net mask for an interface
 *------------------------------------------------------------------------
 */
int setmask(int32_t ifn, IPaddr mask) {
	//IPaddr aobrc;
	IPaddr defmask;

	if (nif[ifn].ni_svalid) {
		/* one set already -- fix things */
		rtdel(nif[ifn].ni_subnet, nif[ifn].ni_mask);
		rtdel(nif[ifn].ni_brc, ip_maskall);
		rtdel(nif[ifn].ni_subnet, ip_maskall);
	}

	nif[ifn].ni_mask = mask ;
	nif[ifn].ni_svalid = true;
	defmask = netmask(nif[ifn].ni_ip);

    nif[ifn].ni_subnet = nif[ifn].ni_ip & nif[ifn].ni_mask;
    if (bsdbrc) {
    	nif[ifn].ni_brc = nif[ifn].ni_subnet;
    } else {
    	nif[ifn].ni_brc = nif[ifn].ni_subnet | (~nif[ifn].ni_mask);
    }
    /* set network (not subnet) broadcast */
    nif[ifn].ni_nbrc = nif[ifn].ni_ip | ~defmask;

    /* install routes */
    /* net */
    rtadd(nif[ifn].ni_subnet, nif[ifn].ni_mask, nif[ifn].ni_ip,
		0, ifn, RT_INF);
	if (bsdbrc) {
		IPaddr	aobrc;		/* all 1's broadcast */

		aobrc = nif[ifn].ni_subnet | ~nif[ifn].ni_mask;
		rtadd(aobrc, ip_maskall, nif[ifn].ni_ip, 0,
			NI_LOCAL, RT_INF);
	} else	/* broadcast (all 1's) */
		rtadd(nif[ifn].ni_brc, ip_maskall, nif[ifn].ni_ip, 0,
			NI_LOCAL, RT_INF);
	/* broadcast (all 0's) */
	rtadd(nif[ifn].ni_subnet, ip_maskall, nif[ifn].ni_ip, 0,
		NI_LOCAL, RT_INF);
	return OK;
}
