#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dgcntl  -  control function for datagram pseudo-devices
 *------------------------------------------------------------------------
 */
int
dgcntl(struct device* devptr, int func, void* arg1) {
	struct dgblk    *dgptr;
	int intr_flag;
	int ret;

	local_intr_save(intr_flag);
	dgptr = (struct dgblk*)devptr->dvioblk;
	ret = OK;

	switch(func) {
		case DG_SETMODE: /* set mode bits */
            dgptr->dg_mode = (int)arg1;
            break;
        case DG_CLEAR:   /* clear queued packets */
            preset(dgptr->dg_xport, freebuf);
            break;
        default:
            ret = SYSERR;
	}

	local_intr_restore(intr_flag);
	return ret;
}
