#ifndef __KERN_PROCESS_PROC_H__
#define __KERN_PROCESS_PROC_H__

#include <defs.h>
#include <list.h>
#include <trap.h>
#include <memlayout.h>
#include <unistd.h>
#include <sem.h>
#include <skew_heap.h>
#include <event.h>


// process's state in his life cycle
enum  proc_state {
	PROC_UNINIT = 0,  // uninitialized
	PROC_SLEEPING,    // sleeping
	PROC_RUNNABLE,    // runnable(maybe running)
	PROC_ZOMBIE,      // almost dead, and wait parent proc to reclaim his resource
    PROC_WAIT_EVENT,  // 调用 ipc_recv event，才会进入这个状态
};


// Saved registers for kernel context switches.
// Don't need to save all the %fs etc. segment registers,
// because they are constant across kernel contexts.
// Save all the regular registers so we don't need to care
// which are caller save, but not the return register %eax.
// (Not saving %eax just simplifies the switching code.)
// The layout of context must match code in switch.S.
struct context {
    uint32_t eip;
    uint32_t esp;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
};

#define PROC_NAME_LEN               50
#define MAX_PROCESS                 4096
#define MAX_PID                     (MAX_PROCESS * 2)
#define BADPID      (-1)    


#define isbadpid(x) ( (x)<=0 || (x) >=MAX_PID )
extern list_entry_t proc_list;

struct inode;

struct proc_struct {
    enum proc_state state;                      // Process state
    int pid;                                    // Process ID
    int runs;                                   // the running times of Process
    uintptr_t kstack;                           // Process kernel stack
    /*
     * 当need_resched==true 时，idle进程在cpu_idle中调用schedule
     * 当need_resched==true 时，用户进程在trap结尾处调用schedule                          
     */
    volatile bool need_resched;                 // bool value: need to be rescheduled to release CPU?
    struct proc_struct *parent;                 // the parent process
    struct mm_struct *mm;                       // Process's memory management field
    struct context context;                     // Switch here to run process
    struct trapframe *tf;                       // Trap frame for current interrupt
    uintptr_t cr3;                              // CR3 register: the base addr of Page Directory Table(PDT)
    uint32_t flags;                             // Process flag
    bool  ptcpumode;                            //  proc is in TCP urgent mode
    char name[PROC_NAME_LEN + 1];               // Process name
    list_entry_t list_link;                     // Process link list 
    list_entry_t hash_link;                     // Process hash list
    int exit_code;                              // exit code (be sent to parent proc)
    uint32_t wait_state;                        // waiting state
    struct proc_struct *cptr, *yptr, *optr;     // relations between processes
    struct run_queue *rq;                       // running queue contains Process
    list_entry_t run_link;                      // the entry linked in run queue
    int time_slice;                             // time slice for occupying the CPU
    skew_heap_entry_t lab6_run_pool;            // FOR LAB6 ONLY: the entry in the run pool
    uint32_t lab6_stride;                       // FOR LAB6 ONLY: the current stride of the process 
    uint32_t lab6_priority;                     // FOR LAB6 ONLY: the priority of process, set by lab6_set_priority(uint32_t)
    event_box_t event_box;                      // the event which process waits   
    struct files_struct *filesp;                // the file related info(pwd, files_count, files_array, fs_semaphore) of process
};

#define PF_EXITING                  0x00000001      // getting shutdown

#define WT_CHILD                    (0x00000001 | WT_INTERRUPTED)
#define WT_TIMER                    (0x00000002 | WT_INTERRUPTED)  // wait timer
#define WT_KSWAPD                    0x00000003                    // wait kswapd to free page
#define WT_KBD                      (0x00000004 | WT_INTERRUPTED)  // wait the input of keyboard
#define WT_KSEM                      0x00000100                    // wait kernel semaphore
#define WT_USEM                     (0x00000101 | WT_INTERRUPTED)  // wait user semaphore
#define WT_EVENT_SEND               (0x00000110 | WT_INTERRUPTED)  // wait the sending event
#define WT_EVENT_RECV               (0x00000111 | WT_INTERRUPTED)  // wait the recving event 
#define WT_MBOX_SEND                (0x00000120 | WT_INTERRUPTED)  // wait the sending mbox
#define WT_MBOX_RECV                (0x00000121 | WT_INTERRUPTED)  // wait the recving mbox
#define WT_PIPE                     (0x00000200 | WT_INTERRUPTED)  // wait the pipe
#define WT_SEM_ALL                  (0x00001000 | WT_INTERRUPTED)  // wake up all process blocked on the sem
#define WT_INTERRUPTED               0x80000000                    // the wait state could be interrupted


extern struct proc_struct *initproc ;
extern struct proc_struct *idleproc ;
extern struct proc_struct *current ;

#define le2proc(le, member)         \
    to_struct((le), struct proc_struct, member)

extern struct proc_struct *idleproc, *inle2procitproc, *current;

void proc_init(void);
void proc_run(struct proc_struct *proc);
int kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags);
int get_initproc_pid();
int get_idleproc_pid();
int32_t getpid();

char *set_proc_name(struct proc_struct *proc, const char *name);
char *get_proc_name(struct proc_struct *proc);
bool set_pid_name(int32_t pid, const char *name);
void cpu_idle(void) __attribute__((noreturn));

struct proc_struct *find_proc(int pid);
void proc_send_ipc(struct proc_struct  *proc);

//syscalls 
int do_fork(uint32_t clone_flags, uintptr_t stack, struct trapframe *tf);
int do_exit(int error_code);
int do_yield(void);
int do_execve(const char *name, int argc, const char **argv);
int do_wait(int pid, int *code_store);
int do_kill(int pid);
int do_brk(uintptr_t *brk_store);
int do_mmap(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);
int do_munmap(uintptr_t addr, size_t len) ;
int do_shmem(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);
//FOR LAB6, set the process's priority (bigger value will get more CPU time) 
void lab6_set_priority(uint32_t priority);
int do_sleep(unsigned int time);
int process_dump();

#endif /* !__KERN_PROCESS_PROC_H__ */

