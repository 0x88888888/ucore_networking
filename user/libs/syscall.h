#ifndef __USER_LIBS_SYSCALL_H__
#define __USER_LIBS_SYSCALL_H__
#include <mboxbuf.h>

int sys_exit(int error_code);
int sys_fork(void);
int sys_wait(int pid, int *store);
int sys_exec(const char *name, int argc, const char **argv);
int sys_yield(void);
int sys_kill(int pid);
int sys_getpid(void);
int sys_brk(uintptr_t *brk_store);
int sys_mmap(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);
int sys_munmap(uintptr_t addr, size_t len);
int sys_putc(int c);
int sys_pgdir(void);
int sys_sleep(unsigned int time);
size_t sys_gettime(void);

struct stat;
struct dirent;

int sys_open(const char *path, uint32_t open_flags, uint32_t arg2);
int sys_close(int fd);
int sys_read(int fd, void *base, size_t len);
int sys_write(int fd, void *base, size_t len);
int sys_seek(int fd, off_t pos, int whence);
int sys_fstat(int fd, struct stat *stat);
int sys_fsync(int fd);
int sys_getcwd(char *buffer, size_t len);
int sys_mkdir(const char *path);
int sys_getdirentry(int fd, struct dirent *dirent);
int sys_dup(int fd1, int fd2);
void sys_lab6_set_priority(uint32_t priority); // only for lab6

int sys_send_event(int pid, int event, unsigned int timeout);
int sys_recv_event(int *pid_store, int *event_store, unsigned int timeout);
int sys_mbox_init(unsigned int max_slots);
int sys_mbox_send(int id, struct mboxbuf *buf, unsigned int timeout);
int sys_mbox_recv(int id, struct mboxbuf *buf, unsigned int timeout);
int sys_mbox_free(int id);
int sys_mbox_info(int id, struct mboxinfo *info);
int sys_receive_packet(uint8_t *buf, size_t len, size_t* len_store);
int sys_transmit_packet(uint8_t *buf, size_t len,size_t* len_store);
int sys_ping(char *target, int len);
int sys_process_dump();
int sys_rtdump();
int sys_arpprint();
int sys_netstatus();

int sys_shmem(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);


int sys_sock_socket(uint32_t type, const char* ipaddr, uint32_t iplen);
int sys_sock_listen(uint32_t tcpfd, uint32_t qsize);
int sys_sock_accept(uint32_t listenfd, uint32_t timeout);
int sys_sock_connect(uint32_t sockfd, const char* ipaddr, uint32_t iplen);
int sys_sock_bind(uint32_t sockfd, uint32_t lport, uint32_t rport);
int sys_sock_send(uint32_t sockfd, char* buf, uint32_t len, uint32_t timeout);
int sys_sock_recv(uint32_t sockfd, char* buf, uint32_t len, uint32_t timeout);
int sys_sock_close(uint32_t sockfd);
int sys_sock_shutdown(uint32_t sockfd, uint32_t type);

#endif /* !__USER_LIBS_SYSCALL_H__ */

