#ifndef __KERN_NET_NET_H__
#define __KERN_NET_NET_H__

#include <sockets.h>
#include <mmu.h>
#include <sem.h>
#include <mutex.h>

extern mutex  network_mtx;

void net_init();

#endif // !__KERN_NET_NET_H__

