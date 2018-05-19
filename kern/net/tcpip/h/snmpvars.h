#ifndef __KERN_NET_TCPIP_H_SNMPVARS_H__
#define __KERN_NET_TCPIP_H_SNMPVARS_H__
/* snmpvars.h */

/* System & Interface MIB */

extern char	*SysDescr;
extern unsigned long  SysUpTime;
extern unsigned long  IfNumber;

/* IP MIB */
extern unsigned long
	IpForwarding, IpDefaultTTL, IpInReceives, IpInHdrErrors,
	IpInAddrErrors, IpForwDatagrams, IpInUnknownProtos, IpInDiscards,
	IpInDelivers, IpOutRequests, IpOutDiscards, IpOutNoRoutes,
	IpReasmTimeout, IpReasmReqds, IpReasmOKs, IpReasmFails, IpFragOKs,
	IpFragFails, IpFragCreates;

/* ICMP MIB */
extern unsigned long
	IcmpInMsgs, IcmpInErrors, IcmpInDestUnreachs, IcmpInTimeExcds,
	IcmpInParmProbs, IcmpInSrcQuenchs, IcmpInRedirects, IcmpInEchos,
	IcmpInEchoReps, IcmpInTimestamps, IcmpInTimestampReps,
	IcmpInAddrMasks, IcmpInAddrMaskReps, IcmpOutMsgs, IcmpOutErrors,
	IcmpOutDestUnreachs, IcmpOutTimeExcds, IcmpOutParmProbs,
	IcmpOutSrcQuenchs, IcmpOutRedirects, IcmpOutEchos,
	IcmpOutEchoReps, IcmpOutTimestamps, IcmpOutTimestampReps,
	IcmpOutAddrMasks, IcmpOutAddrMaskReps;

/* UDP MIB */
extern unsigned long
	UdpInDatagrams, UdpNoPorts, UdpInErrors, UdpOutDatagrams;

/* TCP MIB */
extern unsigned TcpRtoAlgorithm, TcpRtoMin, TcpRtoMax, TcpMaxConn,
	TcpActiveOpens, TcpPassiveOpens, TcpAttemptFails, TcpEstabResets,
	TcpCurrEstab, TcpInSegs, TcpOutSegs, TcpRetransSegs,
        TcpInErrs, TcpOutRsts;

#endif //!__KERN_NET_TCPIP_H_SNMPVARS_H__
