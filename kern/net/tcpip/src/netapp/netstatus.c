#include <tcpip/h/network.h>

static void
print_nif(int i){
    struct netif* ni= &nif[i];
    int j =0;
    cprintf("nif->ni_num     = %x \n",ni->ni_num);
    cprintf("nif->ni_name    = %s \n",ni->ni_name);
    cprintf("nif->ni_state   = %x \n",ni->ni_state);
    cprintf("nif->ni_ip      = %x \n",ni->ni_ip);
    cprintf("nif->ni_gateway = %x \n",ni->ni_gateway);
    cprintf("nif->ni_nserver = %x \n",ni->ni_nserver);
    cprintf("nif->ni_tserver = %x \n",ni->ni_tserver);
    cprintf("nif->ni_net     = %x \n",ni->ni_net);
    cprintf("nif->ni_subnet  = %x \n",ni->ni_subnet);
    cprintf("nif->ni_mask    = %x \n",ni->ni_mask);
    cprintf("nif->ni_brc     = %x \n",ni->ni_brc);
    cprintf("nif->ni_nbrc    = %x \n",ni->ni_nbrc);
    cprintf("nif->ni_mtu     = %x \n",ni->ni_mtu);
    cprintf("nif->ni_hwtype  = %x \n",ni->ni_hwtype);
    cprintf("nif->ni_hwa     =");
    for (j=0; j< EP_ALEN; j++) {
    	cprintf(" %02x",ni->ni_hwa.ha_addr[j]);
    }
    cprintf("\n");
    
    cprintf("nif->ni_hwb     =");
    for (j=0; j< EP_ALEN; j++) {
    	cprintf(" %02x",ni->ni_hwb.ha_addr[j]);
    }
    cprintf("\n");

}

void netstatus() {
  
   print_nif(EC0);

}
