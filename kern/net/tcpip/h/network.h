#ifndef __NET_TCPIP_H_NETWORK_H__
#define __NET_TCPIP_H_NETWORK_H__ 
#include <defs.h>
#include <x86.h>
#include <string.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <slab.h>
#include <assert.h>
#include <sem.h>
#include <mutex.h>
#include <event.h>
#include <mbox.h>
#include <sync.h>
#include <clock.h>
#include <iobuf.h>
#include <inode.h>
#include <proc.h>
#include <dev.h>

#define	OK		 1 		/* returned when system	call ok	*/
#define	SYSERR		-1		/* returned when sys. call fails*/

#define	EOF		-2		/* End-of-file (usu. from read)	*/

#define TIMEOUT     -3L      /* time out (usu. recvtime) */

#define	NULLCH		'\0'		/* The null character		*/
#define	NULLSTR		""		/* Pointer to empty string	*/


#define	LOWBYTE		0377		/* mask for low-order 8 bits	*/
#define	HIBYTE		0177400		/* mask for high 8 of 16 bits	*/


typedef long    tcpseq;

#include <tcpip/h/ports.h>
#include <tcpip/h/queue.h>
#include <tcpip/h/conf.h>
#include <tcpip/h/ip.h>
#include <tcpip/h/ipreass.h>
#include <tcpip/h/ether.h>
#include <tcpip/h/netif.h>

#include <tcpip/h/util.h>

#include <tcpip/h/igmp.h>
#include <tcpip/h/net.h>
#include <tcpip/h/route.h>
#include <tcpip/h/rip.h>
#include <tcpip/h/arp.h>
#include <tcpip/h/icmp.h>
#include <tcpip/h/udp.h>
#include <tcpip/h/dgram.h>
#include <tcpip/h/dhcp.h>

#include <tcpip/h/tcpstat.h>
#include <tcpip/h/tcp.h>
#include <tcpip/h/tcpfsm.h>
#include <tcpip/h/tcb.h>

#include <tcpip/h/tcptimer.h>




//#include <tcpip/h/iobufep.h>
#include <tcpip/h/vars.h>

#include <tcpip/h/socket.h>

#endif // !__NET_TCPIP_H_NETWORK_H__






