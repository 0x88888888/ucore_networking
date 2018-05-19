#ifndef __KERN_NET_TCPIP_H_SOCKET_
#define __KERN_NET_TCPIP_H_SOCKET_


#define SOCK_FREE        0
#define SOCK_BUSY        1

struct socket {
    uint32_t state;     // 0 free, 1,used 
    uint32_t socktype;  // 2 tcp, 3 udp
    uint32_t tcbidx;     //tcbtab 索引
};

#define SOCKTABMAX     100

void init_socket();
int32_t alloc_sock(int32_t type);

#endif // !__KERN_NET_TCPIP_H_SOCKET_
