#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ripout  -  do the RIP route exchange protocol, output side
 *------------------------------------------------------------------------
 */
int32_t 
ripout(void* args) {
	int	rnd =0;
    dorip =true;
	rippid = getpid();
	dorip = true;

	while (true) {
        do_sleep(2000);
        if (++rnd > RIPDELTA)
            rnd = -RIPDELTA;
        lock(&riplock);
        ripsend(ip_maskall, UP_RIP);
        unlock(&riplock);
	}
	panic("ripout not be implemented fully \n");
}

