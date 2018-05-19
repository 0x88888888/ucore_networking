#ifndef __KERN_NET_TCPIP_H_CONF_H__
#define __KERN_NET_TCPIP_H_CONF_H__

#define DEFGATEWAY    "10.0.2.2"
#define MASKSTR       "255.255.255.0"
#define DEFIP         "10.0.2.15"


#define DEBUG_NET   1


/* Device name definitions */
#define	EC0	1		/* class eth	*/
#define	UDP	2		/* class dgm	*/
#define	TCP	11		/* class tcpm	*/

#define EVT_TIME_OUT         5

#define EVT_TYP_IP_EVENT     1
#define EVT_TYP_UDP_EVENT    2



#define	NDEVS	20

#define	isbaddev(f)	( (f)<0 || (f)>=NDEVS )

#define	MEMMARK				/* define if memory marking used*/

#define TCPREXMTTIMEOUT     1000 //10秒

#define	TCPSBS	4096			/* TCP send buffer sizes	*/
#define TCPRBS  4096            /* TCP receive buffer sizes     */


/* Control block sizes */

#define	Neth	1
#define Ndg     16
#define	Ntcp	16

#define	TIMERGRAN  100    /* timer granularity, secs/10	*/

#ifndef	Noth
#define	Noth	0
#endif	//!Noth

extern struct device devtab[];

#endif


