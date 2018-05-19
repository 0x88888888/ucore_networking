#include <mutex.h>


void mutex_init(mutex *m) {
   sem_init(m,1);
}

void lock(mutex *m) {
   down(m);
}

void unlock(mutex *m) {
   up(m);
}

