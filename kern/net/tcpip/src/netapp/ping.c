#include <tcpip/h/network.h>

void ping(char* target, size_t len) {
    int i=0;
    IPaddr dst;
    int ds;     /* data size */
    ds=56;

    if ( (dst=name2ip( target)) == SYSERR) {
        cprintf( "ping: couldn't resolve %s\n", target);
        return ;
    } 

	for(i=0; i<5;i++) {
		icmp(ICT_ECHORQ, 0 , dst, i,  ds);
        panic("ping not be implemented \n");

	}
}

