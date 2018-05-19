#include <tcpip/h/network.h>
#include <e1000.h>
#include <dev.h>

extern int  efaceinit();

#if Noth > 0
static  int getsim();
static  int itod[] = { NI_LOCAL, EC0, OTHER1, OTHER2 };
static  IPaddr  iftoip[] = { {0, 0, 0, 0}, {0, 0, 0, 0},
			{130, 10, 59, 0}, {209, 6, 36, 0} };
#else	/* !(Noth > 0) */
static  int itod[] = { /*-1*/ NI_LOCAL, EC0 };
#endif	/* Noth > 0 */

//iface == 1
int efaceinit(int iface) {

   
	struct etblk *etptr = (struct etblk *)((devtab[nif[iface].ni_dev].dvioblk));
   
    blkcopy(nif[iface].ni_hwa.ha_addr, etptr->etpaddr, EP_ALEN);
	blkcopy(nif[iface].ni_hwb.ha_addr, etptr->etbcast, EP_ALEN);
	cprintf("efaceinit:nif[%d].ni_hwa.ha_addr= %02x:%02x:%02x:%02x:%02x:%02x \n",
		iface,
		nif[iface].ni_hwa.ha_addr[0],
		nif[iface].ni_hwa.ha_addr[1],
		nif[iface].ni_hwa.ha_addr[2],
		nif[iface].ni_hwa.ha_addr[3],
		nif[iface].ni_hwa.ha_addr[4],
		nif[iface].ni_hwa.ha_addr[5]);
	cprintf("efaceinit:nif[%d].ni_hwb.ha_addr= %02x:%02x:%02x:%02x:%02x:%02x \n",
		iface,
		nif[iface].ni_hwb.ha_addr[0],
		nif[iface].ni_hwb.ha_addr[1],
		nif[iface].ni_hwb.ha_addr[2],
		nif[iface].ni_hwb.ha_addr[3],
		nif[iface].ni_hwb.ha_addr[4],
		nif[iface].ni_hwb.ha_addr[5]);

    nif[iface].ni_descr = etptr->etdescr;
	nif[iface].ni_mtype = 6;	/* RFC 1156, Ethernet CSMA/CD	*/
	nif[iface].ni_speed = 10000000;	/* bits per second		*/
	nif[iface].ni_hwa.ha_len = EP_ALEN;
	nif[iface].ni_hwb.ha_len = EP_ALEN;
	nif[iface].ni_mtu = EP_DLEN;
    nif[iface].ni_mcast = NULL;//ethmcast;

    //nif[iface].ni_outp = etptr->etoutp;
    etptr->etintf = iface;
    return OK;
}


/*------------------------------------------------------------------------
 * inithost  --  initialize a gateway's interface structures
 *------------------------------------------------------------------------
 */
int32_t inithost(){
	int i;
	Net.netpool = 0;
	Net.lrgpool = 0;
	mutex_init(&Net.sema);
	Net.nif = NIF; //2
   
	for (i=0; i<Net.nif; ++i) {
		bzero(&nif[i], sizeof(nif[i]));
		nif[i].ni_num = i;    /* index of this nif */
		nif[i].ni_state = NIS_DOWN;
		nif[i].ni_ivalid = nif[i].ni_nvalid = false;
		nif[i].ni_svalid = false;
		nif[i].ni_dev = itod[i]; //devtab中的索引, NI_LOCAL == 0
		nif[i].ni_ipinq = create_queue(20);
		//         0.0.0.0
		nif[i].ni_ip = ip_anyaddr;
		if (i == NI_LOCAL) {
			/* maxlrgbuf - (extended) ether header */
			nif[i].ni_mtu = MAXLRGBUF-EP_HLEN - IP_ALEN;
			continue;
		}

		switch(nif[i].ni_dev) {
			case EC0: //devtab索引 1
			    // i ==1
			    efaceinit(i);  /* for the simulation */
			    nif[i].ni_state = NIS_DOWN;
			    nif[i].ni_admstate = NIS_DOWN;
			    break;
			default:
			    break; 
		}

	}

	return OK;
}


