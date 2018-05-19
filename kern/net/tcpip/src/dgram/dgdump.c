#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dgdump  -  dump the datagram pseudo device control blocks
 *------------------------------------------------------------------------
 */
int
dgdump(void) {
    struct  dgblk   *pdg;
    int     i;

    for(i=0; i<Ndg; i++) {
    	pdg = &dgtab[i];
    	if (pdg->dg_state == DGS_FREE) {
    		continue;
    	}
        cprintf("Dev=%2d: ",pdg->dg_dnum);
        cprintf("lport=%4d, fport=%4d, ", pdg->dg_lport,
            pdg->dg_fport);
        cprintf("mode=%03o, xport=%2d addr=%d.%d.%d.%d\n",
            pdg->dg_mode, pdg->dg_xport,
            (pdg->dg_fip >> 24) & 0xff,
            (pdg->dg_fip >> 16) & 0xff,
            (pdg->dg_fip >> 8) &0377,
            pdg->dg_fip & 0xff);
    }
    return OK;
}

