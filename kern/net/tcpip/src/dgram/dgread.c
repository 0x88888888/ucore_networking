#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dgread  -  read one datagram from a datagram pseudo-device
 *------------------------------------------------------------------------
 */
int
dgread(struct device *pdev, char* pxg0, unsigned int len) {
	struct xgram*   pxg = (struct xgram*)pxg0;
	struct dgblk*   pdg;
	struct upq*     pup;
	struct ep*      pep;
	struct ip*      pip;
	struct udp*     pudp;
	unsigned int    datalen;
    cprintf("dgread len = %d\n",len);
    pdg = (struct dgblk*) pdev->dvioblk;
    
    if (pdg->dg_mode & DG_TMODE) {
        //超时等待
        pup = &upqs[pdg->dg_upq]; 
    	if (!isbadpid(pup->up_pid)) {
    		return SYSERR;
    	}
        //xport上的等待进程的个数
    	if (pcount(pdg->dg_xport) <=0) {
            pup->up_pid = getpid(); //进程去等待
            //等待udp_in中发送过来的数据
            if (-E_TIMEOUT == ipc_event_recv(NULL, EVT_TYP_UDP_EVENT,NULL , 200/*2 sec*/)){
    			pup->up_pid = BADPID;
    			return TIMEOUT;
    		}
    	}
    }
    
    //永远等待数据
    //cprintf("    pdg= %x\n",pdg);
    pep = (struct ep*) preceive(pdg->dg_xport);

    /* copy data into user's buffer & set length */

    pip = (struct ip*)pep->ep_data;
    pudp = (struct udp*)pip->ip_data;
    datalen = pudp->u_len - U_HLEN;
    if (pdg->dg_mode & DG_NMODE) {
    	if (datalen + XGHLEN > len){
            return SYSERR;
    	}
    	pxg->xg_fip = pip->ip_src;
    	pxg->xg_fport = pudp->u_src;
    	pxg->xg_lip = pip->ip_dst;
        pxg->xg_lport = pdg->dg_lport;

    	memcpy(pxg->xg_data, pudp->u_data, datalen);
    } else {
    	if (datalen > len) {
    		datalen = len;
    	}
    	memcpy(pxg, pudp->u_data, datalen);
    }
    freebuf(pep);
    return datalen;
}
