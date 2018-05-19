#ifndef __LIBS_UNISTD_H__
#define __LIBS_UNISTD_H__

#define T_SYSCALL           0x80

/* syscall number */
#define SYS_exit            1
#define SYS_fork            2
#define SYS_wait            3
#define SYS_exec            4
#define SYS_clone           5
#define SYS_yield           10
#define SYS_sleep           11
#define SYS_kill            12
#define SYS_gettime         17
#define SYS_getpid          18
#define SYS_brk             19
#define SYS_mmap            20
#define SYS_munmap          21
#define SYS_shmem           22
#define SYS_putc            30
#define SYS_pgdir           31

#define SYS_event_send      48
#define SYS_event_recv      49
#define SYS_mbox_init       50
#define SYS_mbox_send       51
#define SYS_mbox_recv       52
#define SYS_mbox_free       53
#define SYS_mbox_info       54

#define SYS_open            100
#define SYS_close           101
#define SYS_read            102
#define SYS_write           103
#define SYS_seek            104
#define SYS_fstat           110
#define SYS_fsync           111
#define SYS_getcwd          121
#define SYS_mkdir           122
#define SYS_getdirentry     128
#define SYS_dup             130

//ONLY FOR network
#define SYS_transmit_packet 150
#define SYS_receive_packet  151
#define SYS_ping            152
#define SYS_process_dump    153
#define SYS_rtdump          154
#define SYS_arpprint        155
#define SYS_netstatus       156

#define SYS_sock_socket     200
#define SYS_sock_listen     201
#define SYS_sock_accept     202
#define SYS_sock_connect    203
#define SYS_sock_bind       204
#define SYS_sock_send       205
#define SYS_sock_recv       206
#define SYS_sock_close      207
#define SYS_sock_shutdown   208


/* OLNY FOR LAB6 */
#define SYS_lab6_set_priority 255

/* SYS_fork flags */
#define CLONE_VM            0x00000100  // set if VM shared between processes
#define CLONE_THREAD        0x00000200  // thread group
#define CLONE_FS            0x00000800  // set if shared between processes
 
/* SYS_map flags */
#define MMAP_WRITE          0x00000100
#define MMAP_STACK          0x00000200

/* VFS flags */
// flags for open: choose one of these
#define O_RDONLY            0           // open for reading only
#define O_WRONLY            1           // open for writing only
#define O_RDWR              2           // open for reading and writing
// then or in any of these:
#define O_CREAT             0x00000004  // create file if it does not exist
#define O_EXCL              0x00000008  // error if O_CREAT and the file exists
#define O_TRUNC             0x00000010  // truncate file upon open
#define O_APPEND            0x00000020  // append on each write
// additional related definition
#define O_ACCMODE           3           // mask for O_RDONLY / O_WRONLY / O_RDWR

#define NO_FD               -0x9527     // invalid fd

/* lseek codes */
#define LSEEK_SET           0           // seek relative to beginning of file
#define LSEEK_CUR           1           // seek relative to current position in file
#define LSEEK_END           2           // seek relative to end of file

#define FS_MAX_DNAME_LEN    31
#define FS_MAX_FNAME_LEN    255
#define FS_MAX_FPATH_LEN    4095

#define EXEC_MAX_ARG_NUM    32
#define EXEC_MAX_ARG_LEN    4095

#endif /* !__LIBS_UNISTD_H__ */

