#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  mkarp  -  allocate and fill in an ARP or RARP packet
 *------------------------------------------------------------------------
 */
 static struct ep* mkarp(int32_t ifn, short type, short op, IPaddr spa, IPaddr tpa) {
 	struct arp* parp;
 	struct ep*  pep;
 	pep = (struct ep*)kmalloc(sizeof(struct ep));
 	if (pep == NULL) {
 		return (struct ep*)SYSERR;
 	}

    cprintf("mkarp: nif[%d].ni_hwa.ha_addr= %02x:%02x:%02x:%02x:%02x:%02x \n",
		ifn,
		nif[ifn].ni_hwa.ha_addr[0],
		nif[ifn].ni_hwa.ha_addr[1],
		nif[ifn].ni_hwa.ha_addr[2],
		nif[ifn].ni_hwa.ha_addr[3],
		nif[ifn].ni_hwa.ha_addr[4],
		nif[ifn].ni_hwa.ha_addr[5]);

    cprintf("mkarp: nif[%d].ni_hwb.ha_addr= %02x:%02x:%02x:%02x:%02x:%02x \n",
		ifn,
		nif[ifn].ni_hwb.ha_addr[0],
		nif[ifn].ni_hwb.ha_addr[1],
		nif[ifn].ni_hwb.ha_addr[2],
		nif[ifn].ni_hwb.ha_addr[3],
		nif[ifn].ni_hwb.ha_addr[4],
		nif[ifn].ni_hwb.ha_addr[5]);

 	blkcopy(ep_dst(pep), nif[ifn].ni_hwb.ha_addr, EP_ALEN);
 	blkcopy(ep_src(pep), nif[ifn].ni_hwa.ha_addr, EP_ALEN);

 	ep_type(pep) = type;
 	parp = (struct arp*)pep->ep_data;
 	parp->ar_hwtype = htons(AR_HARDWARE);
 	parp->ar_prtype = htons(EPT_IP);
 	parp->ar_hwlen = EP_ALEN;
 	parp->ar_prlen = IP_ALEN;
 	parp->ar_op = htons(op);

 	blkcopy(SHA(parp), nif[ifn].ni_hwa.ha_addr, EP_ALEN);
    blkcopy(SPA(parp), &spa, IP_ALEN);
    blkcopy(THA(parp), nif[ifn].ni_hwa.ha_addr, EP_ALEN);
    blkcopy(TPA(parp), &tpa, IP_ALEN);
    return pep;
 }

/*------------------------------------------------------------------------
 *  rarpsend  -  broadcast a RARP packet to obtain my IP address
 *------------------------------------------------------------------------
 */
int32_t rarpsend(int32_t ifn)  {
	int intr_flag;
	struct netif* pni = &nif[ifn];
	struct ep *pep;
	int i, mypid, resp;
	IPaddr junk = 0;
	//IPaddr mask;
	//char  buf[16]={0};

	mypid = getpid();
	for (i=0; i< ARP_MAXRETRY; i++) {
		pep=mkarp(ifn, EPT_RARP, RA_REQUEST, junk, junk);
		if (pep == (struct ep*)SYSERR) 
			break;
		local_intr_save(intr_flag);
		rarppid = mypid;
		local_intr_restore(intr_flag);
		//发送arap包
        net_dev_write(pni->ni_dev, pep, EP_MINLEN); //EP_MINLEN=60
        /* ARP_RESEND is in secs, recvtim uses 1/10's of secs	*/
        resp = recvtim(10*ARP_RESEND<<i);
        if (resp !=TIMEOUT) {
        	/* host route */
        	rtadd(pni->ni_ip, ip_maskall, pni->ni_ip, 0, NI_LOCAL, RT_INF);

            /* non-subnetted route */	
            rtadd(pni->ni_net, ip_maskall, pni->ni_ip, 0, NI_LOCAL, RT_INF);
            return OK;
        }

		/*
		netwrite(ifn, pep, EP_MINLEN);
		panic("rarpsend not be implemented fully\n");
		*/
	}
	panic("No response to RARP");
	return SYSERR;

    /*
	cprintf("No RARP response, enter IP addr: \n");
    blkcopy(buf, IPSTR, strlen(IPSTR));
    buf[strlen(IPSTR)]='\0';
    if (name2ip( buf) != SYSERR) {
    	pni->ni_ip = name2ip(buf);
    	pni->ni_net = netnum(pni->ni_ip);
    	pni->ni_ivalid = true; //ip 设置结束

    	mask=netmask(pni->ni_ip);//设置网络掩码
    	setmask(ifn, mask);
    	return OK;
    }
    */
	return OK;
}
