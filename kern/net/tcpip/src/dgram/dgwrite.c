#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dgwrite  -  write one datagram to a datagram pseudo-device
 *------------------------------------------------------------------------
 */
int
dgwrite(struct device* pdev, unsigned char* pxg0, unsigned int len) {
	struct   xgram*   pxg = (struct xgram*)pxg0;
	struct   ep*      pep ;
    struct   ip*      pip;
	struct   udp*     pudp;
    struct   dgblk*   pdg;
    IPaddr      fip;
    unsigned short      fport;  /* destination UDP port */
    
    if (len >U_MAXLEN) {
    	return SYSERR;
    }

    pdg = (struct dgblk*)pdev->dvioblk;
    pep = (struct ep*) kmalloc(sizeof(struct  ep));
    if ((int)pep == SYSERR) {
    	return SYSERR;
    }
    pip = (struct ip*)pep->ep_data;
    pudp = (struct udp*)pip->ip_data;
    fport = pdg->dg_fport;
    fip = pdg->dg_fip;
    if ((pdg->dg_mode & DG_NMODE) != 0) {
        //提供控制机会
    	if (fport == 0) {
    		fport = pxg->xg_fport;
    		fip = pxg->xg_fip;
    	}
        //数据复制进来
    	memcpy(pudp->u_data, pxg->xg_data, len);
    } else {
    	if (fport == 0) {
    		freebuf(pep);
    		return SYSERR;
    	}
    	memcpy(pudp->u_data, pxg, len);
    }
    
    return udpsend(fip, fport, pdg->dg_lport, 
        pep, len, pdg->dg_mode & DG_CMODE );
}
