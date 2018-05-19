#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <rr_sched.h>

static void
rr_init(struct run_queue *rq) {
	list_init(&(rq->run_list));
	rq->proc_num = 0;
}

static void
rr_enqueue(struct run_queue *rq, struct proc_struct *proc) {
	assert(list_empty(&(proc->run_link)));
	list_add_before(&(rq->run_list), &(proc->run_link));
	if (proc->time_slice == 0 || proc->time_slice > rq->max_time_slice) {
		proc->time_slice = rq->max_time_slice;
	}
	proc->rq = rq;
	rq->proc_num++;
}

static void
rr_dequeue(struct run_queue *rq, struct proc_struct *proc) {
    assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
    list_del_init(&(proc->run_link));
    rq->proc_num --;
}

static struct proc_struct *
rr_pick_next(struct run_queue *rq) {
	list_entry_t *le = list_next(&(rq->run_list));
	if (le !=&(rq->run_list)) {
		return le2proc(le, run_link);
	}
    return NULL;
}

static void
rr_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
	if (proc->time_slice > 0) {
		proc->time_slice--;
	}
	if (proc->time_slice == 0) {
		proc->need_resched = 1;
	}
}

struct sched_class rr_sched_class ={
	.name = "rr_scheduler",
	.init = rr_init,
	.enqueue = rr_enqueue,
	.dequeue = rr_dequeue,
	.pick_next = rr_pick_next,
	.proc_tick = rr_proc_tick,
};

