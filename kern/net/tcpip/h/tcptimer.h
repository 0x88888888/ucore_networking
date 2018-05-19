#ifndef __KERN_NET_TCPIP_H_TCPTIMER_H__
#define __KERN_NET_TCPIP_H_TCPTIMER_H__



/* A timer delta list entry */

struct tqent {
	//除了表头之外，其余的 tq_timeleft的值相对于前一项的值
    int   tq_timeleft;    /* time to expire (1/100 secs) */
    long  tq_time;        /* time this entry was queued  */
    int   tq_port;        /* port to send the event      */
    int   tq_portlen;     /* length of "tq_port"         */
    void  *tq_msg;        /* data to send when expired   */
    struct tqent  *tq_next; /* next in the list          */
};
/* timer process declarations and definitions */
extern int32_t tcptimer();
#define TMNAM   "tcptimer"   /* name of fast timer process	*/
 
extern long ctr100;         /* 1/100th of a second clock   */

extern mutex tqmutex;
extern int  tqpid;
extern struct tqent  *tqhead;

int tmclear(int, void *), tmleft(int, void *);
int tmset(int, int, void *, int);
int tcpkilltimers(struct tcb *);

#endif  // !__KERN_NET_TCPIP_H_TCPTIMER_H__

