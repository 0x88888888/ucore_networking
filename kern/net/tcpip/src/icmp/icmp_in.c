#include <tcpip/h/network.h>


/*------------------------------------------------------------------------
 *  icmp_in  -  handle ICMP packet coming in from the network
 *------------------------------------------------------------------------
 */
int
icmp_in(struct netif *pni, struct ep *pep) {
	struct   ip   *pip;
	struct   icmp *pic;
	int i ,len;
    
    pip = (struct ip*)pep->ep_data;
    pic = (struct icmp*)pip->ip_data;

    len = pip->ip_len - IP_HLEN(pip);
    if (cksum((unsigned short*)pic, len)) {
    	IcmpInErrors++;
    	freebuf(pep);
    	return SYSERR;
    }
    IcmpInMsgs++;
    switch(pic->ic_type) {
    	case ICT_ECHORQ:
    	    // echo request
    	    IcmpInEchos++;
    	    return icmp(ICT_ECHORP, 0, pip->ip_src, pep, 0);
		case ICT_ECHORP:
		    // echo reply
		    IcmpInEchoReps++;
		    panic("icmp_in not be implemented fully \n");
		    /*
		     if (send(pic->ic_id, (int)pep) != OK)
			    freebuf(pep);
			*/
		    return OK;
    	case ICT_MASKRQ:
    	    // net mask request
    	    IcmpInAddrMasks++;
    	    if (!gateway) {
    	    	freebuf(pep);
    	    	return OK;
    	    }
    	    pic->ic_type = (char) ICT_MASKRP;
    	    *(IPaddr *)pic->ic_data = netmask(pip->ip_dst);
    	    break;
    	case ICT_MASKRP:
    	    // net mask reply
    	    IcmpInAddrMaskReps++;
    	    for (i=0; i<Net.nif; ++i)
			    if (nif[i].ni_ip ==pip->ip_dst){
				   break;
			    }
		    if (i != Net.nif) {
			    setmask(i, *(IPaddr *)pic->ic_data);
			    panic("icmp_in not be implemented fully \n");
			    //send(pic->ic_id, ICT_MASKRP);
		    }
		    freebuf(pep);
		    return OK;
		case ICT_REDIRECT:
		    // icmp redirect request
		    IcmpInRedirects++;
		    icredirect(pep);
		    return OK;
		case ICT_DESTUR: IcmpInDestUnreachs++;	freebuf(pep); return OK;
	    case ICT_SRCQ:	 IcmpInSrcQuenchs++;	freebuf(pep); return OK;
	    case ICT_TIMEX:	 IcmpInTimeExcds++;	freebuf(pep); return OK;
	    case ICT_PARAMP: IcmpInParmProbs++;	freebuf(pep); return OK;
	    case ICT_TIMERQ: IcmpInTimestamps++;	freebuf(pep); return OK;
	    case ICT_TIMERP: IcmpInTimestampReps++;	freebuf(pep); return OK;
	    default:
		    IcmpInErrors++;
		    freebuf(pep);
		    return OK;
    }
   
    //mask request
    icsetsrc(pip);
    len = pip->ip_len - IP_HLEN(pip);
    pic->ic_cksum = 0;
	pic->ic_cksum = cksum((unsigned short*)pic, len);

	IcmpOutMsgs++;
	ipsend(pip->ip_dst, pep, len, IPT_ICMP, IPP_INCTL, IP_TTL);
    
    return OK;
	//panic("icmp_in not be implemented fully \n");
}

