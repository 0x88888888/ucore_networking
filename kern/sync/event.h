#ifndef __KERN_SYNC_EVENT_H__
#define __KERN_SYNC_EVENT_H__

#include <defs.h>
#include <wait.h>
#include <mutex.h>

typedef struct event_t{
	int event_type;
	int event;
	int sender_pid;
	int send_time;
	struct event_t* pnext;
	struct event_t* pprev;
} event_t;

typedef struct {
   mutex    mtx;
   event_t* pfront;
   event_t* prear;
   wait_t  wait;
} event_box_t;



void event_box_init(struct proc_struct *proc);


bool ipc_event_send(int pid,int evnet_type, int event);
int ipc_event_recv(int *pid_store,int event_type, int *event_store, unsigned int timeout);

#endif /* !__KERN_SYNC_EVENT_H__ */

