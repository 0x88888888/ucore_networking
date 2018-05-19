#include <tcpip/h/network.h>

#ifdef	Ntcp
struct	tcb	tcbtab[Ntcp];		/* tcp device control blocks	*/
#endif

static bool tcpinited=false;

/*------------------------------------------------------------------------
 *  tcpinit  -  devtab中tcp设备初始化
 *------------------------------------------------------------------------
 */
int tcpinit(struct device *pdev) {
	struct	tcb	*tcb;
	
	if (tcpinited==false) {
		tcpinited =true;
		mutex_init(&tcps_tmutex);
		tcps_lqsize = 5; /* default listen Q size */
	}
	pdev->dvioblk = (char *) (tcb = &tcbtab[pdev->dvminor]);
	tcb->tcb_dvnum = pdev->dvnum;
	tcb->tcb_state = TCPS_FREE;
	return OK;
}



