#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  tqdump -  dump a timer queue (for debugging)
 *------------------------------------------------------------------------
 */
int
tqdump(void) {
	/*
	struct	tqent	*tq;

	for (tq=tqhead; tq; tq=tq->tq_next) {
		cprintf("tq <D %d,T %d, (%d, %d)>\n", tq->tq_timeleft,
			tq->tq_time, TCB(tq->tq_msg), EVENT(tq->tq_msg));
	}
	*/
	return OK;
}

/*------------------------------------------------------------------------
 *  tqwrite - write the timer queue to the given device
 *------------------------------------------------------------------------
 */
int tqwrite(int stdout)
{
	struct	tqent	*tq;
	int		timeleft;
	char		str[80];

	if (tqhead == 0) {
		panic("tqwrite not be implemented fully 1\n");
		//write(stdout, "no entries\n", 11);
		return OK;
	}
	panic("tqwrite not be implemented fully 2\n");
	//write(stdout, "timeleft\ttime\tport\tportlen\tmsg\n", 31);
	timeleft = 0;
	for (tq=tqhead; tq; tq=tq->tq_next) {
		timeleft += tq->tq_timeleft;
		/*
		sprintf(str, "%-8d\t%-5d\t%-4d\t%-7d\t%-3d\n",
			timeleft, tq->tq_time, tq->tq_port,
			tq->tq_portlen, tq->tq_msg);
		*/
		panic("tqwrite not be implemented fully 3\n");
		//write(stdout, str, strlen(str));
	}
	return OK;
}


