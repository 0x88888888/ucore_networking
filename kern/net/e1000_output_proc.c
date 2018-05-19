#include <proc.h>
#include <sched.h>
#include <sync.h>
//#include <kmalloc.h>
#include <slab.h>
#include <e1000.h>

#include <mboxbuf.h>
#include <mbox.h>

#include <tcpip/h/network.h>
#include <stdio.h>
#include <net.h>

int e1000_output_proc(void* arg) {
    unlock(&network_mtx);

    return 0;
}









