#ifndef __KERN_NET_H_TCPIP_NET_H__
#define __KERN_NET_H_TCPIP_NET_H__ 
/* High-level network definitions and constants */

#define   NETBUFS     10           /* number of network buffers   */
#define   MAXNETBUF   EP_MAXLEN + EXTRAEPSIZ  /* max network buffer length */
#define   LRGBUFS     5            /* number of large net buffers  */
#define   MAXLRGBUF   BPMAXB       /* size of large net bufs	*/
#define   NETFQ       3            /* size of input-to-output queue */

/* Network input and output processes: procedure name and parameters */
extern int32_t  netinit(), netstart(), slowtimer();
#define NETNAM        "netstart"
#define STNAM         "slowtimer"
#define NETARGC       1           /* count of args to net startup */
#define STARGC        0           /* count of args to net timer   */

struct  netinfo {               /* info and parms. for network  */
        int netpool;            /* network packet buffer pool   */
        int lrgpool;            /* large packet buffer pool */
        int nif;                /* # of configured interfaces   */
        mutex sema;               /* semaphore for global net */
};

extern  struct netinfo Net;     /* All network parameters   */

#endif  //!__KERN_NET_H_TCPIP_NET_H__


