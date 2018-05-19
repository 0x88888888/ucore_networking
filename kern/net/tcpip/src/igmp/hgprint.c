#include <tcpip/h/network.h>



static int32_t	printone(int, struct hg *);

/*------------------------------------------------------------------------
 * hgprint - print the Host Group table on descriptor fd
 *------------------------------------------------------------------------
 */
int32_t hgprint(int fd)
{
	int	i;
	for (i=0; i<HG_TSIZE; ++i)
		printone(fd, &hgtable[i]);
	return OK;
}

/*------------------------------------------------------------------------
 * printone - print one entry in the Host Group table
 *------------------------------------------------------------------------
 */
static int32_t
printone(int fd, struct hg *phg)
{
	/*
	char	line[128], *p;

	if (phg->hg_state == HGS_IDLE)
		strcpy(line, "IDLE ");
	else if (phg->hg_state == HGS_DELAYING)
		strcpy(line, "DELAYING ");
	else
		return OK;
	p = index(line, '\0');

	ip2dot(p, phg->hg_ipa);
	p = index(p, '\0');

	sprintf(p, " intf %d refs %d IPttl %d\n", phg->hg_ifnum,
		phg->hg_refs, phg->hg_ttl);
	return write(fd, line, strlen(line));
	*/
	return OK;
}


