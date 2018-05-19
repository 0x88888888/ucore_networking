#include <tcpip/h/network.h>

mutex rarpsem;
int	rarppid;

struct	arpentry	arptable[ARP_TSIZE];

/*------------------------------------------------------------------------
 *  arpinit  -  initialize data structures for ARP processing
 *------------------------------------------------------------------------
 */
void arpinit() {
	int i;
    mutex_init(&rarpsem);

    rarppid = -1;

	for (i = 0; i < ARP_TSIZE; ++i) {
		arptable[i].ae_state = AS_FREE;
	}
}

