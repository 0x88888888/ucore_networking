#include <defs.h>
#include <stdio.h>
#include <ulib.h>
#include <stat.h>
#include <string.h>
#include <lock.h>
#include <mboxbuf.h>
#include <syscall.h>

static lock_t fork_lock = INIT_LOCK;

void
lock_fork(void) {
    lock(&fork_lock);
}

void
unlock_fork(void) {
    unlock(&fork_lock);
}

void
exit(int error_code) {
    sys_exit(error_code);
    cprintf("BUG: exit failed.\n");
    while (1);
}

int
fork(void) {
    return sys_fork();
}

int
wait(void) {
    return sys_wait(0, NULL);
}

int
waitpid(int pid, int *store) {
    return sys_wait(pid, store);
}

void
yield(void) {
    sys_yield();
}

int
kill(int pid) {
    return sys_kill(pid);
}

int
getpid(void) {
    return sys_getpid();
}

//print_pgdir - print the PDT&PT
void
print_pgdir(void) {
    sys_pgdir();
}

int
mmap(uintptr_t *addr_store, size_t len, uint32_t mmap_flags) {
    return sys_mmap(addr_store, len, mmap_flags);
}

int
munmap(uintptr_t addr, size_t len) {
    return sys_munmap(addr, len);
}

void
lab6_set_priority(uint32_t priority)
{
    sys_lab6_set_priority(priority);
}

int
sleep(unsigned int time) {
    return sys_sleep(time);
}

unsigned int
gettime_msec(void) {
    return (unsigned int)sys_gettime();
}

int
send_event(int pid, int event) {
    return sys_send_event(pid, event, 0);
}

int
send_event_timeout(int pid, int event, unsigned int timeout) {
    return sys_send_event(pid, event, timeout);
}

int
recv_event(int *pid_store, int *event_store) {
    return sys_recv_event(pid_store, event_store, 0);
}

int
recv_event_timeout(int *pid_store, int *event_store, unsigned int timeout) {
    return sys_recv_event(pid_store, event_store, timeout);
}

// 这个是调用 new_mbox,名称不好
int
mbox_init(unsigned int max_slots) {
    return sys_mbox_init(max_slots);
}

int
mbox_send(int id, struct mboxbuf *buf) {
    return sys_mbox_send(id, buf, 0);
}

int
mbox_send_timeout(int id, struct mboxbuf *buf, unsigned int timeout) {
    return sys_mbox_send(id, buf, timeout);
}

int
mbox_recv(int id, struct mboxbuf *buf) {
    return sys_mbox_recv(id, buf, 0);
}

int
mbox_recv_timeout(int id, struct mboxbuf *buf, unsigned int timeout) {
    return sys_mbox_recv(id, buf, timeout);
}

int
mbox_free(int id) {
    return sys_mbox_free(id);
}

int
mbox_info(int id, struct mboxinfo *info) {
    return sys_mbox_info(id, info);
}


int receive_packet(uint8_t *buf, size_t len) {
    size_t len_store;
    //cprintf("user mode receive_packet len =%x %d\n",len,len);
    sys_receive_packet(buf ,len ,&len_store);
    return (int)len_store;
}

int transmit_packet(uint8_t *buf, size_t len, size_t* len_store) { 
    return sys_transmit_packet(buf, len, len_store);
}

int ping(char *target, int len) {
    return sys_ping(target, len);
}

int process_dump() {
    return sys_process_dump();
}

int rtdump() {
    return sys_rtdump();
}

int arpprint() {
    return sys_arpprint();
}

int netstatus() {
    return sys_netstatus();
}

int sock_socket(uint32_t type, const char* ipaddr, uint32_t iplen) {
    return sys_sock_socket(type, ipaddr, iplen);
}

int sock_listen(uint32_t tcpfd, uint32_t qsize) {
    return sys_sock_listen(tcpfd, qsize);
}
int sock_accept(uint32_t listenfd, uint32_t timeout) {
    return sys_sock_accept(listenfd, timeout);
}

int sock_connect(uint32_t sockfd, const char* ipaddr, uint32_t iplen) {
    return sys_sock_connect(sockfd, ipaddr, iplen);
}

int sock_bind(uint32_t sockfd, uint32_t lport, uint32_t rport) {
    return sys_sock_bind(sockfd, lport, rport);
}

int sock_send(uint32_t sockfd, char* buf, uint32_t len, uint32_t timeout) {
    return sys_sock_send(sockfd, buf, len, timeout);
}

int sock_recv(uint32_t sockfd, char* buf, uint32_t len, uint32_t timeout) {
    return sys_sock_recv(sockfd, buf, len, timeout);
}

int sock_close(uint32_t sockfd) {
    return sys_sock_close(sockfd);
}

int sock_shutdown(uint32_t sockfd, uint32_t type) {
    return sys_sock_shutdown(sockfd, type);
}


int
__exec(const char *name, const char **argv) {
    int argc = 0;
    while(argv[argc] != NULL) {
        argc ++;
    }
    return sys_exec(name, argc, argv);
}

