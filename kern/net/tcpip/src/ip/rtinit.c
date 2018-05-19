#include <tcpip/h/network.h>

struct rtinfo Route=  {
	.ri_default = NULLPTR,
	.ri_bpool  = 0,
	.ri_valid =false,
}; //{NULLPTR, 0, 0 ,0};

/* The initialization above seems to be necessary, I don't know why */ 
struct route *rttable[RT_TSIZE];

/*------------------------------------------------------------------------
 *  rtinit  -  initialize the routing table
 *------------------------------------------------------------------------
 */
void rtinit()
{   

	int i=0;
	for (i=0;i<RT_TSIZE; ++i) {
		rttable[i] = NULL;
	}

	mutex_init(&Route.ri_mutex);
	Route.ri_bpool = 0;
	Route.ri_valid = true;
	Route.ri_default = NULL;
	cprintf("rtinit dwon\n");
}

