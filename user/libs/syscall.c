#include <defs.h>
#include <unistd.h>
#include <stdarg.h>
#include <stat.h>
#include <dirent.h>
#include <stdio.h>

//#include <mboxbuf.h>
#include <syscall.h>
#define MAX_ARGS            5

static inline int
syscall(int num, int a1, int a2, int a3, int a4, int a5) {
    if (num == SYS_receive_packet) {
        cprintf("user mode SYS_receive_packet a1=%x a2=%x\n", a1,a2);
    }
    int ret;
    asm volatile (
        "int %1;"
        : "=a" (ret)
        : "i" (T_SYSCALL),
          "a" (num),
          "d" (a1),
          "c" (a2),
          "b" (a3),
          "D" (a4),
          "S" (a5)
        : "cc", "memory");
    return ret;
}

/*
static inline int
syscall(int num, ...) {
    va_list ap;
    va_start(ap, num);
    uint32_t a[MAX_ARGS];
    int i, ret;
    for (i = 0; i < MAX_ARGS; i ++) {
        a[i] = va_arg(ap, uint32_t);
        if (num == SYS_receive_packet){
            cprintf("a[%d]=%x \n",i,a[i]);
        }
    }
    va_end(ap);
    
    asm volatile (
        "int %1;"
        : "=a" (ret)
        : "i" (T_SYSCALL),
          "a" (num),
          "d" (a[0]),
          "c" (a[1]),
          "b" (a[2]),
          "D" (a[3]),
          "S" (a[4])
        : "cc", "memory");
    return ret;
}
*/


int
sys_exit(int error_code) {
    return syscall(SYS_exit, error_code, 0, 0 , 0, 0);
}

int
sys_fork(void) {
    return syscall(SYS_fork,0, 0 , 0, 0, 0);
}

int
sys_wait(int pid, int *store) {
    return syscall(SYS_wait, pid, (int)store,0, 0 , 0);
}

int
sys_yield(void) {
    return syscall(SYS_yield, 0, 0 , 0, 0, 0);
}

int
sys_kill(int pid) {
    return syscall(SYS_kill, pid, 0, 0 , 0, 0);
}

int
sys_getpid(void) {
    return syscall(SYS_getpid, 0, 0 , 0, 0, 0);
}

int
sys_brk(uintptr_t *brk_store) {
    return syscall(SYS_brk, (int)brk_store, 0, 0 , 0, 0);
}

int
sys_mmap(uintptr_t *addr_store, size_t len, uint32_t mmap_flags) {
    return syscall(SYS_mmap, (int)addr_store, len, mmap_flags, 0, 0 );
}

int
sys_munmap(uintptr_t addr, size_t len) {
    return syscall(SYS_munmap, addr, len, 0, 0 , 0);
}

int
sys_shmem(uintptr_t *addr_store, size_t len, uint32_t mmap_flags) {
    return syscall(SYS_shmem, (int)addr_store, len, mmap_flags, 0, 0);
}

int
sys_putc(int c) {
    return syscall(SYS_putc, c, 0, 0 , 0, 0);
}

int
sys_pgdir(void) {
    return syscall(SYS_pgdir, 0, 0 , 0, 0, 0);
}

void
sys_lab6_set_priority(uint32_t priority)
{
    syscall(SYS_lab6_set_priority, priority, 0, 0 , 0, 0);
}

int
sys_sleep(unsigned int time) {
    return syscall(SYS_sleep, time, 0, 0 , 0, 0);
}

size_t
sys_gettime(void) {
    return syscall(SYS_gettime, 0, 0 , 0, 0, 0);
}

int
sys_exec(const char *name, int argc, const char **argv) {
    return syscall(SYS_exec, (int)name, argc, (int)argv, 0, 0);
}

int
sys_open(const char *path, uint32_t open_flags, uint32_t arg2) {
    return syscall(SYS_open, (int)path, open_flags, arg2, 0 , 0);
}

int
sys_close(int fd) {
    return syscall(SYS_close, fd, 0, 0 , 0, 0);
}

int
sys_read(int fd, void *base, size_t len) {
    return syscall(SYS_read, fd, (int)base, len, 0, 0 );
}

int
sys_write(int fd, void *base, size_t len) {
    return syscall(SYS_write, fd, (int)base, len, 0, 0 );
}

int
sys_seek(int fd, off_t pos, int whence) {
    return syscall(SYS_seek, fd, pos, whence, 0, 0 );
}

int
sys_fstat(int fd, struct stat *stat) {
    return syscall(SYS_fstat, fd, (int)stat, 0, 0 , 0);
}

int
sys_fsync(int fd) {
    return syscall(SYS_fsync, fd, 0, 0 , 0, 0);
}

int
sys_getcwd(char *buffer, size_t len) {
    return syscall(SYS_getcwd, (int)buffer, len, 0, 0, 0);
}

int
sys_mkdir(const char *path) {
    return syscall(SYS_mkdir, (int)path, 0, 0, 0, 0);
}

int
sys_getdirentry(int fd, struct dirent *dirent) {
    return syscall(SYS_getdirentry, fd, (int)dirent, 0, 0 , 0);
}

int
sys_dup(int fd1, int fd2) {
    return syscall(SYS_dup, fd1, fd2, 0, 0 , 0);
}

int 
sys_receive_packet(uint8_t *buf, size_t len, size_t* len_store) {
    cprintf("user mode sys_receive_packet len =%x\n",len);
    return syscall(SYS_receive_packet, (int)buf, len, (int)len_store, 0, 0 );
}

int
sys_transmit_packet(uint8_t *buf, size_t len,size_t* len_store) {
    cprintf("user mode transmit_packet len =%x\n",len);

    return syscall(SYS_transmit_packet, (int)buf, len, (int)len_store, 0, 0);
}


int
sys_send_event(int pid, int event, unsigned int timeout) {
    return syscall(SYS_event_send, pid, event, timeout, 0,0);
}

int
sys_recv_event(int *pid_store, int *event_store, unsigned int timeout) {
    return syscall(SYS_event_recv, (int)pid_store, (int)event_store, timeout, 0,0);
}

int
sys_mbox_init(unsigned int max_slots) {
    return syscall(SYS_mbox_init, max_slots, 0,0, 0,0);
}

int
sys_mbox_send(int id, struct mboxbuf *buf, unsigned int timeout) {
    return syscall(SYS_mbox_send, id, (int)buf, timeout, 0,0);
}

int
sys_mbox_recv(int id, struct mboxbuf *buf, unsigned int timeout) {
    return syscall(SYS_mbox_recv, id, (int)buf, timeout, 0,0);
}

int
sys_mbox_free(int id) {
    return syscall(SYS_mbox_free, id, 0,0, 0,0);
}

int
sys_mbox_info(int id, struct mboxinfo *info) {
    return syscall(SYS_mbox_info, id, (int)info, 0,0, 0);
}

int 
sys_ping(char *target, int len) {
   return syscall(SYS_ping, (int)target, len, 0, 0, 0);
}

int 
sys_process_dump() {
    return syscall(SYS_process_dump, 0, 0, 0, 0, 0);
}

int
sys_rtdump() {
    return syscall(SYS_rtdump,0, 0, 0, 0, 0);
}

int sys_arpprint() {
    return syscall(SYS_arpprint, 0, 0, 0, 0, 0);
}

int sys_netstatus() {
    return syscall(SYS_netstatus, 0, 0, 0, 0, 0);
}

int sys_sock_socket(uint32_t type, const char* ipaddr, uint32_t iplen) {
    return syscall(SYS_sock_socket, type, (int)ipaddr , iplen, 0, 0);
}

int sys_sock_listen(uint32_t tcpfd, uint32_t qsize) {
    return syscall(SYS_sock_listen, tcpfd, qsize, 0, 0, 0);
}

int sys_sock_accept(uint32_t listenfd, uint32_t timeout) {
    return syscall(SYS_sock_accept, listenfd, timeout, 0, 0, 0);
}

int sys_sock_connect(uint32_t sockfd, const char* ipaddr, uint32_t iplen) {
    return syscall(SYS_sock_connect, sockfd, (int)ipaddr, iplen, 0, 0);
}

int sys_sock_bind(uint32_t sockfd, uint32_t lport, uint32_t rport) {
    return syscall(SYS_sock_bind, sockfd, lport, rport, 0, 0);
}

int sys_sock_send(uint32_t sockfd, char* buf, uint32_t len, uint32_t timeout) {
    return syscall(SYS_sock_send, sockfd, (int)buf, len, timeout, 0);
}

int sys_sock_recv(uint32_t sockfd, char* buf, uint32_t len, uint32_t timeout) {
    return syscall(SYS_sock_recv, sockfd, (int)buf, len, timeout, 0);
}

int sys_sock_close(uint32_t sockfd) {
    return syscall(SYS_sock_close, sockfd, 0, 0, 0, 0);
}

int sys_sock_shutdown(uint32_t sockfd, uint32_t type) {
    return syscall(SYS_sock_shutdown, sockfd, type, 0, 0, 0);
}





