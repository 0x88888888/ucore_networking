#ifndef __LIBS_SOCKETS_H__
#define __LIBS_SOCKETS_H__
#include <x86.h>
#include <defs.h>

struct in_addr {
	uint32_t s_addr;
};

struct sockaddr_in {
   uint8_t sin_len;
   uint8_t sin_family;
   uint16_t sin_port; //2 bytes
   struct in_addr sin_addr; //4 bytes
   char sin_zero[8];
};

struct sockaddr {
	uint8_t sa_len;
	uint8_t sa_family;
	char sa_data[14];
};

#ifndef socklen_t
#define socklen_t uint32_t
#endif

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3



#endif // !__LIBS_SOCKETS_H__



