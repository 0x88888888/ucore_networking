#ifndef __KERN_SYNC_MUTEX_H__
#define __KERN_SYNC_MUTEX_H__
#include <sem.h>

typedef semaphore_t mutex;

void mutex_init(mutex *m);
void lock(mutex *m);
void unlock(mutex *m);



#endif // __KERN_SYNC_MUTEX_H__

