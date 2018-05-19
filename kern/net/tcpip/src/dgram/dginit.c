#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  dginit  -  initialize datagram protocol pseudo device marking it free
 *------------------------------------------------------------------------
 */
#ifdef  Ndg
//这个表在 系统初始化的时候就分配给了devtab中相应的 dev了
struct  dgblk dgtab[Ndg];		/* dg device control blocks	*/
#endif
mutex dgmutex;

int
dginit(struct device* pdev) {
	struct dgblk   *pdg;

	pdev->dvioblk = (char*)(pdg = &dgtab[pdev->dvminor]);
	pdg->dg_dnum = pdev->dvnum; //记录在 devtab中的索引 
	pdg->dg_state = DGS_FREE;
	mutex_init(&dgmutex);
	return OK;
}

