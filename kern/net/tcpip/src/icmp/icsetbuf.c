#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  icsetbuf -  set up a buffer for an ICMP message
 *------------------------------------------------------------------------
 */
struct ep* icsetbuf(int type, char *pa1, bool *pisresp, bool *piserr) 
{ 
    struct ep *pep;
    *piserr = *pisresp = false;

    switch(type) {
        case ICT_REDIRECT:
            pep = (struct ep*)kmalloc(sizeof(struct ep));
            if (pep == NULL) {
            	return (NULL);
            }
            blkcopy(pep, pa1, MAXNETBUF);
            pa1 = (char *) pep; // ?
            *piserr = true;
            break;
        case ICT_DESTUR:
        case ICT_SRCQ:
        case ICT_TIMEX:
        case ICT_PARAMP:
            pep = (struct ep*)pa1;
            *piserr = true;
            break;
        case ICT_ECHORP:
        case ICT_INFORP:
        case ICT_MASKRP:
            pep = (struct ep *)pa1;
            *pisresp = true; //应答报文
            break;
        case ICT_ECHORQ:
        case ICT_TIMERQ:
        case ICT_INFORQ:
        case ICT_MASKRQ:
            pep = (struct ep*)kmalloc(sizeof (struct ep));
            if (pep == NULL){
            	return NULL;
            }
            break;
        case ICT_TIMERP:    /* Not Implemented */
            /* IcmpOutTimestampsReps++; */
            IcmpOutErrors--;
            freebuf(pa1);
            return NULL;
    }

    switch (type) {		/* Update MIB Statistics */
        case ICT_ECHORP:	IcmpOutEchos++;		break;
        case ICT_ECHORQ:	IcmpOutEchoReps++;	break;
        case ICT_DESTUR:	IcmpOutDestUnreachs++;	break;
        case ICT_SRCQ:		IcmpOutSrcQuenchs++;	break;
        case ICT_REDIRECT:	IcmpOutRedirects++;	break; 
        case ICT_TIMEX:		IcmpOutTimeExcds++;	break;
        case ICT_PARAMP:	IcmpOutParmProbs++;	break;
        case ICT_TIMERQ:	IcmpOutTimestamps++;	break;
        case ICT_TIMERP:	IcmpOutTimestampReps++;	break;
        case ICT_MASKRQ:	IcmpOutAddrMasks++;	break;
        case ICT_MASKRP:	IcmpOutAddrMaskReps++;	break;
	}
    return pep;
}


