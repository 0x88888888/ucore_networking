#ifndef __KERN_NET_TCPIP_H_IPREASS_H__
#define __KERN_NET_TCPIP_H_IPREASS_H__

#define  IP_FQSIZE      10   /* max number of frag queues    */
#define  IP_MAXNF       10   /* max number of frags/datagram */
#define  IP_FTTL        60   /* time to live(secs)           */

/* ipf_state flags   */
 
#define IPFF_VALID    1      /* contents are valid   */
#define IPFF_BOGUS    2      /* drop frag that match */
#define IPFF_FREE     3      /* this queue is free to be allocated */

struct  ipfq   {
    char     ipf_state;         /* VALID, FREE or BOGUS     */
    IPaddr   ipf_src;           /* IP address of the source */
    short    ipf_id;            /* ip_id              */
    int      ipf_ttl;           /* countdown to disposal    */
    queue    *ipf_q;             /* the queue of fragments   */
};

extern  mutex  ipfmutex;        /* mutex for ipfqt[]		*/
extern  struct ipfq  ipfqt[];   /* IP frag queue table		*/

#endif //!__KERN_NET_TCPIP_H_IPREASS_H__




