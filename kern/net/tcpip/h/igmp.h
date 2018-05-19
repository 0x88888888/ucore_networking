#ifndef __KERN_NET_TCPIP_H_IGMP_G__
#define __KERN_NET_TCPIP_H_IGMP_G__
#define  HG_TSIZE       15    /* host group table size   */

#define IG_VERSION      1     /* RFC 1112 version number */

#define IG_HLEN         8     /* IGMP header length      */
//igmp报文类型
#define IGT_HQUERY      1     /* host membership query   */
#define IGT_HREPORT     2     /* host membership report  */

struct igmp {
    unsigned char  ig_vertyp; /* igmp协议版本和igmp报文类型  */
    char           ig_unused; /* igmp_v1不使用        */
    unsigned short ig_cksum;  /* compl. of 1's compl. sum */
    IPaddr         ig_gaddr;  /* host group IP address   */
};

#define IG_VER(pig)     (((pig)->ig_vertyp>>4) & 0xf)
#define IG_TYP(pig)     ((pig)->ig_vertyp & 0xf)

//加入一个组时，需要发送igmp成员关系报告的个数
#define IG_NSEND        2     /* # IGMP join messages to send	*/
#define IG_DELAY        5     /* delay for resends (1/10 secs)*/

/* Host Group Membership States */ 

#define HGS_FREE        0     /* unallocated host group table entry */
#define HGS_DELAYING    1     /* delay timer running for this group */
#define HGS_IDLE        2     /* in the group but no report pending */
#define HGS_STATIC      3     /* for 224.0.0.01; no state changes   */

struct hg {
    unsigned char   hg_state; /* HGS_* above */
    unsigned char   hg_ifnum; /* interface index for group */
    IPaddr          hg_ipa;   /* IP multicast address      */
    unsigned long   hg_refs;  /* reference count           */
    bool            hg_ttl;   /* max IP ttl for this group */
};

/* Host Group Update Process Info.  */

extern int32_t      igmp_update();
#define IGUNAM      "igmp_update"     /* name of update  process	*/
#define	IGUARGC		0		/* count of args to hgupdate	*/

struct hginfo {
    bool   hi_valid;  /* true if hginit() has been called */
    mutex  hi_mutex;  /* table mutual excual exclusion    */
    int    hi_uport;  /* listen port for delay timer expires */
};

extern IPaddr  ig_allhosts;   /* "all hosts" group address (224.0.0.1) */
extern IPaddr  ig_allDmask;   /* net mask to match all class D addrs.	*/
extern struct  hg hgtable[];

extern int hgjoin(unsigned int, IPaddr, bool);
extern int hgadd(unsigned int, IPaddr, bool);
extern int hgarpadd(unsigned int ifnum, IPaddr ipa);
extern int hgleave(unsigned int, IPaddr);
extern int hgarpdel(unsigned int, IPaddr);
extern struct hg *hglookup(unsigned int, IPaddr);
extern int igmp(int, unsigned int, IPaddr);
extern int igmp_settimers(unsigned int);
extern int hgrand(void);

#endif //!__KERN_NET_TCPIP_H_IGMP_G__

 