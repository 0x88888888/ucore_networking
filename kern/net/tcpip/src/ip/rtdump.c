#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  rtdump  -  dump the routing table to the console
 *------------------------------------------------------------------------
 */
 void rtdump() {
 	struct route *srt;
 	int i;

 	if (!Route.ri_valid) {
 		rtinit();
 	}
 	lock(&Route.ri_mutex);
    cprintf("net        mask             gateway    metric intf ttl  refs  use\n");
 
    for (i=0; i<RT_TSIZE; i++) {
    	for (srt =rttable[i]; srt != NULL; srt =srt->rt_next) {
            cprintf("%d.%d.%d.%d  %d.%d.%d.%d  %d.%d.%d.%d  %4d    %1d %4d %4d %4d\n",
            	        (srt->rt_net&0xff000000)>>24,
                        (srt->rt_net&0x00ff0000)>>16,
                        (srt->rt_net&0x0000ff00)>>8,
                        (srt->rt_net&0x000000ff),

                        (srt->rt_mask&0xff000000)>>24,
                        (srt->rt_mask&0x00ff0000)>>16,
                        (srt->rt_mask&0x0000ff00)>>8,
                        (srt->rt_mask&0x000000ff),

                        (srt->rt_gw&0xff000000)>>24,
                        (srt->rt_gw&0x00ff0000)>>16,
                        (srt->rt_gw&0x0000ff00)>>8,
                        (srt->rt_gw&0x000000ff),

                        srt->rt_metric,
                        srt->rt_ifnum,
                        srt->rt_ttl,
                        srt->rt_refcnt,
                        srt->rt_usecnt
                    );

            /*
            cprintf("%08x %08x %08x %4d    %1d %4d %4d %4d\n",
                ((long )srt->rt_net),
                ((long )srt->rt_mask),
                ((long )srt->rt_gw),
				srt->rt_metric,
				srt->rt_ifnum,
				srt->rt_ttl,
				srt->rt_refcnt,
				srt->rt_usecnt);
            */
    	}
    }
    
	if (Route.ri_default){
		cprintf("default route:\n");
		srt=Route.ri_default;
		cprintf("%d.%d.%d.%d  %d.%d.%d.%d  %d.%d.%d.%d  %4d    %1d %4d %4d %4d\n",
            	        (srt->rt_net&0xff000000)>>24,
                        (srt->rt_net&0x00ff0000)>>16,
                        (srt->rt_net&0x0000ff00)>>8,
                        (srt->rt_net&0x000000ff),

                        (srt->rt_mask&0xff000000)>>24,
                        (srt->rt_mask&0x00ff0000)>>16,
                        (srt->rt_mask&0x0000ff00)>>8,
                        (srt->rt_mask&0x000000ff),

                        (srt->rt_gw&0xff000000)>>24,
                        (srt->rt_gw&0x00ff0000)>>16,
                        (srt->rt_gw&0x0000ff00)>>8,
                        (srt->rt_gw&0x000000ff),

                        srt->rt_metric,
                        srt->rt_ifnum,
                        srt->rt_ttl,
                        srt->rt_refcnt,
                        srt->rt_usecnt
                    );
		/*
		cprintf("%08x %08x %08x %4d    %1d %4d %4d %4d\n",
            ((long )Route.ri_default->rt_net),
            ((long )Route.ri_default->rt_mask),
            ((long )Route.ri_default->rt_gw),
			Route.ri_default->rt_metric,
			Route.ri_default->rt_ifnum,
			Route.ri_default->rt_ttl,
			Route.ri_default->rt_refcnt,
			Route.ri_default->rt_usecnt);
	   */  
	}
    
    unlock(&Route.ri_mutex);
}


