#ifndef __KERN_SYNC_SEM_H__
#define __KERN_SYNC_SEM_H__

#include <defs.h>
#include <atomic.h>
#include <wait.h>

typedef struct {
    int value;
    wait_queue_t wait_queue;
} semaphore_t;

void sem_init(semaphore_t *sem, int value);
void up(semaphore_t *sem);
void down(semaphore_t *sem);
bool try_down(semaphore_t *sem);
int  sem_val(semaphore_t *sem);
uint32_t sem_wait_count(semaphore_t *sem);
void wakeup_all(semaphore_t *sen);


#define wait(x)       down((x))
#define try_wait(x)   try_down((x))
#define signal(x)     up((x))

#endif /* !__KERN_SYNC_SEM_H__ */

