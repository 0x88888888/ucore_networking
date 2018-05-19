#ifndef __KERN_NET_TCPIP_H_TCPSTAT_H__
#define __KERN_NET_TCPIP_H_TCPSTAT_H__ 


/*	The union returned by the TCP STATUS control call		*/
struct tcpstat {
    int  ts_type ;     /* which kind of TCP status? */
    union {
        struct {
        	long      tsu_connects;   /* # connections	*/
        	long      tsu_aborts;     /* # aborts		*/
        	long      tsu_retrans;    /* # retransmissions	*/
        } T_unt;  
        struct {
            IPaddr    tsu_laddr;      /* local IP         */
            short     tsu_lport;      /* local TCP port   */
            IPaddr    tsu_faddr;      /* foreign IP       */
            short     tsu_fport;      /* foreign TCP port */
            short     tsu_rwin;       /* receive window	  */
            short     tsu_swin;       /* peer's window    */
            short     tsu_state;      /* TCP state        */
            long      tsu_unacked;    /* bytes unacked    */
            int       tsu_prec;       /* IP precedence    */
        } T_unc;
        struct {
            long      tsu_requests;   /* # connect requests	*/
            long      tsu_qmax;       /* max queue lenght */
        } T_uns;
    } T_un;
};

#define	ts_connects	T_un.T_unt.tsu_connects
#define	ts_aborts	T_un.T_unt.tsu_aborts
#define	ts_retrans	T_un.T_unt.tsu_retrans

#define	ts_laddr	T_un.T_unc.tsu_laddr
#define	ts_lport	T_un.T_unc.tsu_lport
#define	ts_faddr	T_un.T_unc.tsu_faddr
#define	ts_fport	T_un.T_unc.tsu_fport
#define	ts_rwin		T_un.T_unc.tsu_rwin
#define	ts_swin		T_un.T_unc.tsu_swin
#define	ts_state	T_un.T_unc.tsu_state
#define	ts_unacked	T_un.T_unc.tsu_unacked
#define	ts_prec		T_un.T_unc.tsu_prec

#define	ts_requests	T_un.T_uns.tsu_requests
#define	ts_qmax		T_un.T_uns.tsu_qmax




#endif // !__KERN_NET_TCPIP_H_TCPSTAT_H__


