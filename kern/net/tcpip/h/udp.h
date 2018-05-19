#ifndef __KERN_NET_TCPIP_H_UDP_H__
#define __KERN_NET_TCPIP_H_UDP_H__

/* User Datagram Protocol (UDP) constants and formats */

/* udp头长度 8字节*/
#define  U_HLEN     8     

/* udp最大的数据长度 	*/
#define	U_MAXLEN	(IP_MAXLEN-(IP_MINHLEN<<2)-U_HLEN)

struct udp {        /* message format of DARPA UDP	*/
    unsigned short u_src;             /* source UDP port number	*/
    unsigned short u_dst;             /* destination UDP port number	*/
    unsigned short u_len;             /* length of UDP data    */
    unsigned short u_cksum;           /* UDP checksum (0 => none)   */
    unsigned char  u_data[U_MAXLEN];  /* data in UDP message   */
};

/* UDP constants */
#define   ULPORT         2050   /* initial UDP local "port" number	*/

/* assigned UDP port numbers */
#define	UP_ECHO		7	/* echo server				*/
#define	UP_DISCARD	9	/* discard packet			*/
#define	UP_USERS	11	/* users server				*/
#define	UP_DAYTIME	13	/* day and time server			*/
#define	UP_QOTD		17	/* quote of the day server		*/
#define	UP_CHARGEN	19	/* character generator			*/
#define	UP_TIME		37	/* time server				*/
#define	UP_WHOIS	43	/* who is server (user information)	*/
#define	UP_DNAME	53	/* domain name server			*/
#define	UP_TFTP		69	/* trivial file transfer protocol server*/
#define UP_RPC      111 /* SUN RPC */
#define UP_RWHO     513 /* remote who server (ruptime)      */
#define	UP_RIP		520	/* route information exchange (RIP)	*/

#define	UPPS		Ndg	/* number of ports used to demultiplex udp datagrams	*/
#define	UPPLEN		50	/* size of a demux queue		*/

#define	UDP_DHCP_CPORT  68      /* Port number for DHCP client	*/
#define	UDP_DHCP_SPORT  67      /* Port number for DHCP server	*/

/* mapping of external network UDP "port" to internal Xinu port */

struct upq {    /* UDP demultiplexing info	*/
    bool              up_valid;  /* is this entry in use?	*/
    unsigned short    up_port;   /* local UDP port number	*/
    int               up_pid;    /* process for waiting reader	*/
    int               up_xport;  /* corresponding port on which incoming pac, queued */
} ;

#endif //!__KERN_NET_TCPIP_H_UDP_H__
