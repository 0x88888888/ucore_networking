#include <tcpip/h/network.h>



struct ptnode   *ptfree;        /* list of free queue nodes */
struct pt       ports[NPORTS];  
int ptnextp;

static bool inited=false;

void _ptclear(struct pt *ppt, int newstate, void (*dispose)());

/*------------------------------------------------------------------------
 *  pinit  --  initialize all ports
 *------------------------------------------------------------------------
 */
int32_t portinit(int maxmsgs) {
	int i;
	struct ptnode *next, *prev;
	if (inited ==false) {
	    
	    //弄出 maxmsgs个 msg node 
		if ( (ptfree=(struct ptnode *) kmalloc(maxmsgs * sizeof(struct ptnode))) == NULL) {
			panic("pinit - insufficient memory");
		}
	    memset(ptfree, 0, maxmsgs * sizeof(struct ptnode));
	    memset(ports, 0, sizeof(ports));

	    //初始化nports个port出来
	    for (i=0; i<NPORTS; i++) {
	    	ports[i].ptstate = PTFREE;
	    }
	    
	    //最后一个
	    ptnextp = NPORTS - 1;
	    
	    /* link up free list of message pointer nodes */
	    
	    //连接好 msg  ptnode
	    for ( prev=next=ptfree; --maxmsgs > 0; ) {
	    	prev->ptnext = ++next;
	    	prev =next;
	    }
	    
	    //最后一个ptnode
	    prev->ptnext = (struct ptnode*)NULL;
	    inited = true;
    }
    return OK;
}

//分配一个pt
int32_t pcreate(unsigned int count) {
    int intr_flag;
    int i , p;
    struct pt  *ptptr;

    local_intr_save(intr_flag);

    
    for (i=0; i < NPORTS ;i++) {
    	if ( (p=ptnextp--) <= 0) {
    		ptnextp = NPORTS - 1;//循环
    	}
    	if ( (ptptr= &ports[p])->ptstate == PTFREE) {
    		ptptr->ptstate = PTALLOC;   //已分配状态
    		sem_init(&(ptptr->ptssem), count); //生产者
    		sem_init(&(ptptr->ptrsem), 0);//消费者
    		ptptr->pthead = ptptr->pttail = (struct ptnode *)NULL;
    		ptptr->ptct = 0;
    		ptptr->ptmaxcnt = count;
    		local_intr_restore(intr_flag);
    		return (p);//port索引
    	}
    }
    local_intr_restore(intr_flag);
    return SYSERR;
}

/*------------------------------------------------------------------------
 *  pcount  --  return the count of current messages in a port
 *------------------------------------------------------------------------
 */
int32_t pcount(int portid) {
	int intr_flag;
	int count;
	struct pt  *ptptr;
    
    local_intr_save(intr_flag);

    if ( isbadport(portid) || (ptptr= &ports[portid])->ptstate != PTALLOC ) {
			local_intr_restore(intr_flag);
			return 0;
	}
      
    count = ptptr->ptct;

    local_intr_restore(intr_flag);
    return count;
} 

/*------------------------------------------------------------------------
 *  pdelete  --  delete a port, freeing waiting processes and messages
 *               归还pt
 *------------------------------------------------------------------------
 */
int32_t pdelete(int portid,void (*dispose)(void* arg))
{
	int intr_flag;    
	struct	pt *ptptr;

	local_intr_save(intr_flag);
	if ( isbadport(portid) || (ptptr= &ports[portid])->ptstate != PTALLOC ) {
		local_intr_restore(intr_flag);
		return SYSERR;
	}
	_ptclear(ptptr, PTFREE, dispose);
	local_intr_restore(intr_flag);
	return OK;
}

/*------------------------------------------------------------------------
 *  preset  --  reset a port, freeing waiting processes and messages
 *------------------------------------------------------------------------
 */
int32_t preset(int portid, void (*dispose)(void *arg)) {
	int intr_flag;    
	struct	pt *ptptr;

	local_intr_save(intr_flag);
	if ( isbadport(portid) || (ptptr= &ports[portid])->ptstate != PTALLOC ) {
		local_intr_restore(intr_flag);
		return(SYSERR);
	}
	_ptclear(ptptr, PTALLOC, dispose);
	local_intr_restore(intr_flag);
	return(OK);
}

/*------------------------------------------------------------------------
 *  _ptclear  --  used by pdelete and preset to clear a port
 *------------------------------------------------------------------------
 */
void _ptclear(struct pt *ppt, int newstate, void (*dispose)())
{
	struct	ptnode	*p;

	/* put port in limbo until done freeing processes */
	ppt->ptstate = PTLIMBO;
	if ( (p=ppt->pthead) != (struct ptnode *)NULL ) {
		//逐个处理
		for(; p != (struct ptnode *) NULL ; p=p->ptnext){
			if (dispose != NULL)
			   (*dispose)( p->ptmsg );//通常来讲，dispose可以是freebuf
		}

		(ppt->pttail)->ptnext = ptfree;
		ptfree = ppt->pthead;
	}
	if (newstate == PTALLOC) {
		//panic("in _ptclear , not support newstate == PTALLOC\n");
		
		ppt->pttail = ppt->pthead = (struct ptnode *) NULL;
		wakeup_all(&(ppt->ptssem));
		wakeup_all(&(ppt->ptrsem));
        sem_init(&(ppt->ptssem), ppt->ptmaxcnt);
        sem_init(&(ppt->ptrsem), 0);
        ppt->ptct = 0;
	} else {
		wakeup_all(&(ppt->ptssem));
		wakeup_all(&(ppt->ptrsem));
	}
	ppt->ptstate = newstate;
}

//在pt上等待信息到来
int32_t 
preceive(int portid) {
	int intr_flag;
	struct pt  *ptptr;
	int msg;
	struct ptnode *nxtnode;

	local_intr_save(intr_flag);
	if (isbadport(portid) || (ptptr= &ports[portid])->ptstate != PTALLOC ) {
        local_intr_restore(intr_flag);
        return (SYSERR);
	}

	wait(&(ptptr->ptrsem));
    nxtnode = ptptr->pthead;
	msg = nxtnode->ptmsg;
	nxtnode->ptmsg = NULL;
	if (ptptr->pthead == ptptr->pttail){ 
		//ptptr中只有一个消息
		ptptr->pthead->ptnext = ptptr->pttail->ptnext= NULL;
		ptptr->pthead = ptptr->pttail = (struct ptnode *)NULL;
	} else {
		//ptptr中有多个消息
		ptptr->pthead = nxtnode->ptnext; /* return to free list	*/
	}
	//归还到free中去
	nxtnode->ptnext = ptfree;		
	ptfree = nxtnode;
    ptptr->ptct--;
	signal(&(ptptr->ptssem));
	local_intr_restore(intr_flag);
	return(msg);
}

//发送信息到pt
int32_t 
psend(int portid, int msg) {
	int intr_flag;    
	struct	pt	*ptptr;
	struct	ptnode	*freenode;
	
	local_intr_save(intr_flag);
	
	if ( isbadport(portid) || (ptptr= &ports[portid])->ptstate != PTALLOC ) {
		local_intr_restore(intr_flag);
		return(SYSERR);
	}

	wait(&(ptptr->ptssem));
	if (ptfree == NULL) {
		panic("Ports  -  out of nodes");
	}
    
    //从ptfree上摘下一个ptnode来
	freenode = ptfree;
	ptfree  = freenode->ptnext;
    
	freenode->ptnext = (struct ptnode *) NULL;
	freenode->ptmsg  = msg;
	//将消息挂到链表中去
	if (ptptr->pttail == (struct ptnode *) NULL){	/* empty queue */
		ptptr->pttail = ptptr->pthead = freenode;
	}else {
		(ptptr->pttail)->ptnext = freenode;
		ptptr->pttail = freenode;
	}
	ptptr->ptct++;
	//通知接收进程
	signal(&(ptptr->ptrsem));
	local_intr_restore(intr_flag);
	return(OK);
}


