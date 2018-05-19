#include <tcpip/h/network.h>



struct socket sockets[SOCKTABMAX];

mutex socktab_mtx;

void 
init_socket() {
    int i;
    for (i=0; i < SOCKTABMAX; i++) {
    	sockets[i].state = SOCK_FREE;
    }
    mutex_init(&socktab_mtx);
}

int32_t 
alloc_sock(int32_t type) {
	int32_t ret = -1, i;
	lock(&socktab_mtx);
	for (i=0; i < SOCKTABMAX; i++) {
		if (sockets[i].state == SOCK_FREE) {
			sockets[i].state = SOCK_BUSY;
            ret = i;
            break;
		}
	}
    unlock(&socktab_mtx);
    return ret;
}

int32_t
sock_socket(int32_t type, const char* ip, uint32_t len) {
    /*
    char* kip= (char*)kmalloc(len+1);
    if (kip != NULL) {
        bzero(kip, len+1);
        memcpy(kip, ip, len);
        struct file *file;
        int ret;
        if ((ret = fd_array_alloc(NO_FD, &file)) != 0) {
            return ret;
        }

        //找打fd了，然后需要联系fd和inode, tcb,udp

        return file->fd;
    }
    */
    return -1;
    
}

