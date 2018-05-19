#include <tcpip/h/network.h>

extern bool dorip; // true if we're running RIP output
extern int  rippid; // rip output pid, if running

/*------------------------------------------------------------------------
 * rttimer - update ttls and delete expired routes
 *------------------------------------------------------------------------
 */

void
rttimer(unsigned int delta) {
	struct route *prt, *prev;
	bool   ripnotify;
	int    i;

	if (!Route.ri_valid) {
		return;
	}
	lock(&(Route.ri_mutex));
	ripnotify = false;
	for (i=0; i < RT_TSIZE; i++) {
        if (rttable[i] == 0) {
           continue;
        }
        for (prev = NULL , prt =rttable[i]; prt != NULL;) {
        	if (prt->rt_ttl != RT_INF) {
        		prt->rt_ttl -= delta;
        	}
        	if (prt->rt_ttl <= 0) {
               if (dorip && prt->rt_metric < RTM_INF) {
               	  prt->rt_metric = RTM_INF;
               	  prt->rt_ttl = RIPZTIME;
               	  ripnotify = true;
               	  continue;
               }

               if (prev != NULL) {
                  prev->rt_next = prt->rt_next;
                  RTFREE(prt);
                  prt = prev->rt_next;
               } else {
               	  rttable[i] = prt->rt_next;
               	  RTFREE(prt);
               	  prt = rttable[i];
               } 
               continue;
        	}
            prev = prt;
            prt = prt->rt_next;
        }
	}
	prt = Route.ri_default;
	if (prt && (prt->rt_ttl < RT_INF) && (prt->rt_ttl -= delta) <= 0) {
		if (dorip && prt->rt_metric < RTM_INF) {
			prt->rt_metric = RTM_INF;
			prt->rt_ttl = RIPZTIME;
		} else {
			RTFREE(Route.ri_default);
			Route.ri_default = 0;
		}
	}
    unlock(&(Route.ri_mutex));

    if (dorip && ripnotify) {
    	panic("rttimer not be implemented fully\n");
    	send(rippid, 0);/* send anything but TIMEOUT	*/
    }
     
}
