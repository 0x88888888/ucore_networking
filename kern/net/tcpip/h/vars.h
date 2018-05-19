#ifndef __KERN_NET_TCPIP_H_VARS_H__
#define __KERN_NET_TCPIP_H_VARS_H__

/*****************************vars*****************************/
extern struct rtinfo Route;
extern IPaddr	ip_anyaddr;
extern IPaddr	ip_loopback;
extern IPaddr	ip_maskall;
extern struct   route *rttable[RT_TSIZE];
extern  mutex  ipfmutex;        /* mutex for ipfqt[]		*/
extern  struct ipfq  ipfqt[];   /* IP frag queue table		*/

extern int	rippid ;
extern bool dorip ;
extern mutex riplock;

//snmpvars
#include <tcpip/h/snmpvars.h>

//udp
mutex udpmutex;

//igmp
extern struct  hginfo  HostGroup;
extern struct  hg      hgtable[];

/*****************************functions*****************************/
//proc && timer
extern int32_t netstart(void* args);
extern int32_t slowtimer_proc(void* args);
extern int32_t ipproc(void* args);
extern int32_t tcpout(void* args);
extern int32_t tcpinp(void* args);
extern int32_t ripout(void* args) ;
extern int32_t rip(void* args);
extern int32_t tcptimer(void* args);
extern int32_t udpechod(void* args);
extern int32_t tcpechod(void* args);

//timers
extern void rttimer(unsigned int delta);

//net
extern int32_t inithost();
extern int netmatch(IPaddr dst, IPaddr net, IPaddr mask, bool islocal);
extern int ni_in(struct netif *pni,struct ep *pep, int len);
extern IPaddr name2ip(char* nam);
extern IPaddr netnum(IPaddr ipa);
extern short cksum(unsigned short* buf, int nwords);
extern int32_t netwrite(struct netif* pni,struct ep* pkt, int32_t count) ;
extern IPaddr netmask(IPaddr net);
extern int setmask(int32_t inum, IPaddr mask);
extern int local_out(struct ep* pep);
extern IPaddr name2ip(char *nam);

//arp/rarp
extern void arpinit();
extern int arp_in(struct netif *pni, struct ep *pep);
extern int rarp_in(struct netif *pni, struct ep *pep);
extern struct arpentry* arpadd(struct netif *pni, struct arp* parp);
extern struct arpentry* arpalloc();
extern void arpdq(struct arpentry *pae);
extern struct arpentry* arpfind(unsigned char *pra, int prtype, struct netif *pni);
extern int arpprint();
extern void arpqsend(struct arpentry *pae);
extern int arpsend(struct arpentry* pae);
extern int32_t getaddr();
extern int32_t getiaddr(int inum);
extern int32_t rarpsend(int32_t ifn);
extern void arptimer(int gran);

//ip

extern int ipputp(int inum, IPaddr nexthop, struct ep* pep) ;
extern struct ep* ipfcons(struct ipfq* iq);
extern int ipfhcopy(struct	ep* pepto,struct ep* pepfrom, unsigned int offindg);
extern struct ep* ipfjoin(struct ipfq* iq);
extern void ipftimer(int gran );
extern int ipfsend(struct netif* pni, IPaddr nexthop, struct ep* pep, int offset, int maxdlen, int offindg);
extern bool ipfadd(struct ipfq* iq, struct ep* pep);
extern int ipdstopts(struct netif* pni, struct ep *pep);
extern struct ep* ipreass(struct ep *pep);
extern void ipfinit();
extern int ip_in(struct netif *pni, struct ep *pep);
extern int32_t ipsend(IPaddr faddr, struct ep* pep, int datalen, unsigned char proto, unsigned char ptos, unsigned ttl);
extern struct ip* ipnet2h(struct ip *pip);
extern struct ip *iph2net(struct ip* pip);
extern void ipredirect(struct ep *pep, int ifnum, struct route *prt);
extern struct ep* ipgetp(int* pifnum);
extern void ipdbc(int ifnum, struct ep *pep, struct route* prt);
extern uint32_t getnext_ip_id();

//router
extern int isbrc(IPaddr dest);
extern void rtinit();
extern int rthash(IPaddr net);
extern struct route* rtnew(IPaddr net, IPaddr mask, IPaddr gw,int mertric, int ifnum, int ttl);
extern int rtfree(struct route *prt);
extern void rtdump();
extern struct route *rtget(IPaddr dest, bool local);
extern int rtdel(IPaddr net, IPaddr mask);
extern int rtadd(IPaddr net, IPaddr mask, IPaddr gw, int metric, int intf, int ttl) ;




//icmp
extern int32_t icerrok(struct ep *pep);
extern int32_t icmp(int type, int code, IPaddr dst, void* pa1, void* pa2);
extern struct ep* icsetbuf(int type, char *pa1, bool *pisresp, bool *piserr);
extern void icsetsrc(struct ip *pip);
extern int icsetdata(int type, struct ip *pip, char *pa);
extern int icmp_in(struct netif *pni, struct ep *pep);
extern int icredirect(struct ep *pep);

//udp
extern mutex udpmutex;
extern	struct	upq	upqs[];
extern void udph2net(struct udp* pudp);
extern void udpnet2h(struct udp* pudp);
extern unsigned short udpcksum(struct ep *pep, int len);
extern int udpsend(IPaddr fip, unsigned short fport, unsigned short lport,
	       struct ep* pep, unsigned datalen, bool docksum);
extern unsigned short udpnxtp();
extern int upalloc();
extern int udp_in(struct netif* pni, struct ep* pep);

//dgram
extern int dgalloc();
extern int dgmopen(struct device *pdev, void* fspec0, void* lport0);
extern int dgmcntl(struct device* pdev, int com, void* arg1);
extern int dgclose(struct device *pdev);
extern int dgcntl(struct device* devptr, int func, void* arg1) ;
extern int dgread(struct device *pdev, char* pxg0, unsigned int len);
extern int dgwrite(struct device* pdev, unsigned char* pxg0, unsigned int len);
extern int dginit(struct device* pdev);
extern int dgdump(void);
extern int32_t recvtim(int maxwait);
extern int dnparse(char *fspec, IPaddr *paddr, unsigned short *pport);

//dhcp
extern uint32_t getlocalip(int nif_num);

//igmp
extern int hgjoin(unsigned int, IPaddr, bool);
extern int hgadd(unsigned int, IPaddr, bool);
extern int hgleave(unsigned int, IPaddr);
extern int hgarpdel(unsigned int, IPaddr);
extern struct hg *hglookup(unsigned int, IPaddr);
extern int igmp(int, unsigned int, IPaddr);
extern int igmp_settimers(unsigned int);
extern int hgrand(void);
extern int igmp_in(struct netif* pni, struct ep *pep);

//rip
extern int ripcheck(struct rip *prip ,int len);
extern int ripsend(IPaddr gw, unsigned short port) ;
extern int ripadd(struct rq rqinfo[], struct route* prt) ;
extern int ripifset(struct rq rqinfo[], IPaddr gw, unsigned short port );
extern int ripmetric(struct route *prt, int ifnum);
extern int ripok(struct riprt *rp);
extern int riprecv(struct rip* prip, int len, IPaddr gw);
extern int ripstart(struct rq *prq);
extern int riprepl(struct rip *pripin, unsigned len, IPaddr gw, unsigned short port);


//tcp
extern int tcp_in(struct netif *pni, struct ep *pep);
extern int tcpackit(struct tcb *ptcb, struct ep *pepin);
extern unsigned short tcpcksum(struct ep* pep, IPaddr saddr, IPaddr daddr);
extern unsigned short tcpcksum2(IPaddr saddr, IPaddr daddr, unsigned short proto ,uint16_t tcplen, void* pudp_or_tcp) ;
extern uint16_t calculate_checksum(void *addr, int count,uint32_t start_sum);

extern struct tcb* tcpdemux(struct ep *pep);
extern struct tcp * tcph2net(struct tcp *ptcp);
extern struct tcp * tcpnet2h(struct tcp *ptcp);
extern bool tcpok(struct tcb *ptcb, struct ep *pep);
extern int tcpreset(struct ep *pepin);
extern int tcprwindow(struct tcb *ptcb);
extern int tcpclosed(struct tcb *ptcb, struct ep *pep);
extern int tcprmss(struct tcb *ptcb, struct ip *pip);

//tcpd
extern int tcpinit(struct device *pdev);
extern int tcpclose(struct device* pdev);
extern int tcpwrite(struct device *pdev, unsigned char *pch, unsigned int len);
extern int tcpread(struct device *pdev, char *pch, unsigned int len);
extern int tcpcntl(struct device *pdev, int func, void *arg, void* arg2);
extern int tcpmopen(struct device *pdev, void *fport0, void *lport0);
extern int tcpmcntl(struct device *pdev, int func, void *arg1);


//device functions

extern int32_t init(int descrp);//初始化net device 对象
extern int32_t net_dev_close(int descrp);
extern int32_t net_dev_control(int descrp, int func, ...);
extern int net_dev_open(int descrp, const void *nam0, const void *mode0);
extern int net_dev_read(int descrp, void *buff, unsigned int count);
extern int net_dev_write(int descrp, void *buff, unsigned count);

extern int error_init(struct device *dev);
extern int error_open(struct device *dev, uint32_t open_flags, uint32_t arg2);
extern int error_close(struct device *dev) ;
extern int error_io(struct device *dev, struct iobuf *iob, bool write);
extern int error_ioctl(struct device *dev, int op, void* arg1, void* arg2) ;
extern int error_write(struct device *dev, char* buff, int len);
extern int error_read(struct device *dev, char* buff, int len);


extern int dev_e1000_init(struct device *dev);
extern int dev_e1000_open(struct device *dev, uint32_t open_flags, uint32_t arg2);
extern int dev_e1000_close(struct device *dev);
extern int dev_e1000_write(struct device *dev, char* buff, int len);
extern int dev_e1000_read(struct device *dev, char* buff, int len);
extern int dev_e1000_ioctl(struct device *dev, int op, void* arg1, void* arg2);
extern int dev_e1000_io(struct device *dev, struct iobuf *iob, bool write);

extern int tcp_init(struct device *dev);
extern int tcp_open(struct device *dev, uint32_t open_flags, uint32_t arg2);
extern int tcp_close(struct device *dev);
extern int tcp_write(struct device *dev, char* buff, int len);
extern int tcp_read(struct device *dev, char* buff, int len);
extern int tcp_ioctl(struct device *dev, int op, void* arg1, void* arg2);
extern int tcp_io(struct device *dev, struct iobuf *iob, bool write);

extern int udp_init(struct device* dev);
extern int udp_open(struct device *dev, uint32_t open_flags, uint32_t arg2);
extern int udp_close(struct device *dev);
extern int udp_write(struct device *dev, char* buff, int len);
extern int udp_read(struct device *dev, char* buff, int len);
extern int udp_ioctl(struct device *dev, int op, void* arg1, void* arg2);
extern int udp_io(struct device *dev, struct iobuf *iob, bool write);


extern int null_init(struct device *dev);
extern int null_open(struct device *dev, uint32_t open_flags, uint32_t arg2);
extern int null_close(struct device *dev);
extern int null_write(struct device *dev, char* buff, int len);
extern int null_read(struct device *dev, char* buff, int len);
extern int null_ioctl(struct device *dev, int op, void* arg1, void* arg2);
extern int null_io(struct device *dev, struct iobuf *iob, bool write);


extern int udp_m_open(struct device *dev, uint32_t fport, uint32_t lport);
extern int udp_m_iocntl(struct device *dev, int op, void* arg1, void* arg2);

extern int tcp_m_open(struct device *dev, uint32_t fport, uint32_t lport);
extern int tcp_m_iocntl(struct device *dev, int op, void* arg1, void* arg2);



extern int dump_protocol(struct ep* pep);
extern void ping(char* target, size_t len);

#endif // !__KERN_NET_TCPIP_H_VARS_H__

