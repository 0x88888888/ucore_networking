#include <defs.h>
#include <wait.h>
#include <proc.h>
#include <vmm.h>
#include <ipc.h>
#include <sync.h>
#include <slab.h>
#include <assert.h>
#include <error.h>
#include <clock.h>
#include <event.h>
#include <stdio.h>

void
event_box_init(struct proc_struct *proc) {
    mutex_init(&(proc->event_box.mtx));
    proc->event_box.pfront =proc->event_box.prear =NULL;
    wait_init(&(proc->event_box.wait), proc);
}

static event_t* 
get_event(int event_type,int event,int sender_pid) {

    event_t* eve = kmalloc(sizeof(event_t));
    eve->event_type = event_type;
    eve->event = event;
    eve->sender_pid = sender_pid;
    eve->send_time = gettime2();
    return eve;
}

static void 
free_event(event_t *eve) {
    if (eve != NULL) {
        kfree(eve);
    }
}

static bool
is_empty_event_box(event_box_t* box){
    return box->pfront == NULL ? true : false;
}

static bool
add_event(event_box_t* box, event_t *pevent) {
   if(!box || !pevent) {
    return false;
   }

   if (is_empty_event_box(box)) {
     box->pfront=box->prear=pevent;

     pevent->pnext = pevent->pprev = NULL;
   } else {
     //插到最前面
     box->pfront->pprev = pevent;
     pevent->pnext= box->pfront;

     box->pfront = pevent;
   }
   return true;
}

static event_t*
del_event(event_box_t* box, int type) {
    if (is_empty_event_box(box)) {
       return NULL;
    }

    //event_t* ret =false;
    event_t* found_event=NULL;
    event_t* event = box->pfront;

    while(event) {
      if(event->event_type == type) {
        found_event = event;
        break;
      }
    }
    
    if(found_event == box->pfront) {
        //头
        if(box->pfront == box->prear) {
            //只有一个
            box->pfront = box->prear = NULL;
            return found_event;
        }
        box->pfront = found_event->pnext;
        return found_event;
    } else if(found_event == box->prear) {
        found_event->pprev->pnext = NULL;
        box->prear= found_event->pprev;

        return found_event;
    } else if (found_event != NULL) {
        found_event->pprev->pnext = found_event->pnext;

        found_event->pnext->pprev = found_event->pprev;
        return found_event;
    }
    
    return NULL;
}




int
ipc_event_recv(int *pid_store,int event_type, int *event_store, unsigned int timeout){   
    long now=0;
    event_t* eve = del_event(&(current->event_box), event_type);
    long starttime = gettime2();
    if (eve != NULL) {
       goto recvd;
    }

    while(true) {
          schedule();
          eve = del_event(&(current->event_box), event_type);
          if(eve != NULL){
            goto recvd;// return 
          }

          now= gettime2();
          
          if (now - starttime >= timeout){
            //cprintf("recv: is_empty_event_box? %d\n",is_empty_event_box(&(current->event_box)));
            return -E_TIMEOUT;
          }
    }

recvd:
    if(eve != NULL) {
        if(pid_store != NULL) {
            *pid_store = eve->sender_pid;
        }
        if(event_store != NULL) {
            *event_store = eve->event;
        }
        free_event(eve);
    }
    return 0;
    //return ipc_check_timeout(timeout, saved_ticks);
}

bool 
ipc_event_send(int pid,int evnet_type, int event){
    struct proc_struct *proc;
    if ((proc = find_proc(pid)) == NULL || proc->state == PROC_ZOMBIE) {
        return -E_INVAL;
    }

    if (proc == current || proc == idleproc || proc == initproc ) {
        //自己不能给自己发消息，idleproc和initproc不能接受消息
        cprintf("ipc_event_send 2  %d %d %d\n",proc==current, proc==idleproc,proc==initproc);
        return -E_INVAL;
    }
    
    event_t* eve = get_event(evnet_type, event, current->pid);

    event_box_t* box= &proc->event_box;
    add_event(box,eve);
    cprintf("send: is_empty_event_box? %d\n",is_empty_event_box(&(proc->event_box)));

    return true; //ipc_check_timeout(timeout, saved_ticks);
}