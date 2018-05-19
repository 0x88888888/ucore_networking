#include <tcpip/h/network.h>


#define HDR1 \
"Intf.   IP address     Physical Address   HW  Proto   State       TTL   qcnt\n"
#define HDR2 \
"----- ---------------  -----------------  --  -----  --------  -------  ------\n"

static int	printone(struct arpentry *);

/*------------------------------------------------------------------------
 * arpprint - print the ARP table on descriptor fd
 *------------------------------------------------------------------------
 */
int
arpprint()
{
	int	i;

	//write(fd, HDR1, strlen(HDR1));
	//write(fd, HDR2, strlen(HDR2));
	cprintf("\n");
	cprintf(HDR1);
	cprintf(HDR2);
	for (i = 0; i<ARP_TSIZE; ++i){
		if (arptable[i].ae_state != AS_FREE){
			printone(&arptable[i]);
		}
	}
	return OK;
}

char	*index(const char *, char);

/*------------------------------------------------------------------------
 * printone - print one entry in the ARP table
 *------------------------------------------------------------------------
 */

static int
printone(struct arpentry *pae)
{
	
	char	line[128];
	int     *ip;
	ip=(int*)pae->ae_pra;
    //snprintf()
    //nif
    
	snprintf(line, 128, " %2d    ", pae->ae_pni - &nif[0]);
	cprintf(line);
    
    //snprintf(line, 128, " %2d.%2d.%2d.%2d   ", pae->ae_pra[0],pae->ae_pra[1],pae->ae_pra[2],pae->ae_pra[3]);
    snprintf(line, 128, " %02x.%02x.%02x.%02x   ", ((*ip)&0xff000000) >>24,
                                                   ((*ip)&0x00ff0000) >>16,
                                                   ((*ip)&0x0000ff00) >>8,
                                                   ((*ip)&0x000000ff) );
	cprintf(line);
    memset(line,0,128);
    snprintf(line, 128, " %02x:%2x:%02x:%02x:%02x:%02x  ", pae->ae_hwa[0],
    	                            pae->ae_hwa[1],
    	                            pae->ae_hwa[2],
    	                            pae->ae_hwa[3],
    	                            pae->ae_hwa[4],
    	                            pae->ae_hwa[5]);
	cprintf(line);

	memset(line,0,128);
	snprintf(line, 128,"%02d  ", pae->ae_hwtype);
    cprintf(line);

    memset(line,0,128);
	snprintf(line, 128,"%04x    ", pae->ae_prtype);
    cprintf(line);

    memset(line,0,128);
	snprintf(line, 128,"%02d         ", pae->ae_state);
    cprintf(line);

    memset(line,0,128);
	snprintf(line, 128,"%d    ", pae->ae_ttl);
    cprintf(line);
	
	memset(line,0,128);
	snprintf(line, 128,"%d      \n", qcount(pae->ae_queue));
    cprintf(line);

	return OK;// write(fd, line, strlen(line));
}


