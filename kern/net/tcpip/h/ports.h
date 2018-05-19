#ifndef __KERN_NET_TCPIP_PORTS_H__
#define __KERN_NET_TCPIP_PORTS_H__
 
 
#define	MAXMSGS		50		/* maximum messages on all ports*/
#define	PTFREE		'\01'		/* port is Free			*/
#define	PTLIMBO		'\02'		/* port is being deleted/reset	*/
#define	PTALLOC		'\03'		/* port is allocated		*/
#define	PTEMPTY		-1		/* initial semaphore entries	*/

#define	PTNODISP	0		/* ptclear() null disposal func */

#ifndef	NPORTS
#define NPORTS  16
#endif	//!NPORTS

struct ptnode {   /* node on list of message ptrs	*/
    int ptmsg;    /* a one-word message		*/
    struct ptnode  *ptnext;  /* address of next node on list	*/
};

struct pt {              /* entry in the port table	*/
    char ptstate;        /* port state (FREE/LIMBO/ALLOC)*/
    //mutex mtx;           //操作 pthead , pttail之前，先lock到mtx
    semaphore_t ptssem;  /* 生产者 sender semaphore  */
    semaphore_t ptrsem;  /* 消费者 receiver semaphore */
    
    int   ptmaxcnt;      /* max message to be queued */
    int   ptct;          /* no. of messages in queue	*/
    struct	ptnode	*pthead;	/* list of message pointers	*/
	struct	ptnode	*pttail;	/* tail of message list		*/
};

extern struct ptnode  *ptfree;  /* list of free nodes       */
extern struct pt    ports[];    /* port table               */
extern int ptnextp;             /* next port to examine when looking for a free one */

#define isbadport(portid)       ( (portid)<0 || (portid)>=NPORTS )

extern int32_t portinit();
extern int32_t pdelete(int, void (*dispose)(void *arg));
extern int32_t pcreate(unsigned int);
extern int32_t preset(int, void (*dispose)(void *arg));
extern int32_t pcount(int);
extern int32_t preceive(int);
extern int32_t psend(int, int);
//extern int32_t recvtim(int);

#endif // !__KERN_SYNC_PORT_H__

