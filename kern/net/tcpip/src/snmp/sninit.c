/* sninit.c - sninit */

#include <tcpip/h/network.h>


extern char vers[];
extern long tod;

char 	 *SysDescr;
//struct oid SysObjectID = { {0}, 1};

/* System & Interface MIB */
unsigned long     SysUpTime;
unsigned long     IfNumber;

/* IP MIB */
unsigned long IpForwarding, IpDefaultTTL, IpInReceives, IpInHdrErrors,
	IpInAddrErrors, IpForwDatagrams, IpInUnknownProtos, IpInDiscards,
	IpInDelivers, IpOutRequests, IpOutDiscards, IpOutNoRoutes,
	IpReasmTimeout, IpReasmReqds, IpReasmOKs, IpReasmFails, IpFragOKs,
	IpFragFails, IpFragCreates;

/* ICMP MIB */
unsigned long IcmpInMsgs, IcmpInErrors, IcmpInDestUnreachs, IcmpInTimeExcds,
	IcmpInParmProbs, IcmpInSrcQuenchs, IcmpInRedirects, IcmpInEchos,
	IcmpInEchoReps, IcmpInTimestamps, IcmpInTimestampReps,
	IcmpInAddrMasks, IcmpInAddrMaskReps, IcmpOutMsgs, IcmpOutErrors,
	IcmpOutDestUnreachs, IcmpOutTimeExcds, IcmpOutParmProbs,
	IcmpOutSrcQuenchs, IcmpOutRedirects, IcmpOutEchos,
	IcmpOutEchoReps, IcmpOutTimestamps, IcmpOutTimestampReps,
	IcmpOutAddrMasks, IcmpOutAddrMaskReps;

/* UDP MIB */
unsigned long UdpInDatagrams, UdpNoPorts, UdpInErrors, UdpOutDatagrams;

/* TCP MIB */
unsigned TcpRtoAlgorithm, TcpRtoMin, TcpRtoMax, TcpMaxConn,
	TcpActiveOpens, TcpPassiveOpens, TcpAttemptFails, TcpEstabResets,
	TcpCurrEstab, TcpInSegs, TcpOutSegs, TcpRetransSegs,
        TcpInErrs, TcpOutRsts;

int snmpinitialized = false;

/*------------------------------------------------------------------------
 * sninit - initialize the data structures for the SNMP server and client
 *------------------------------------------------------------------------
 */
void sninit()
{
    panic("sninit is not be implemented \n");

	/*
	int	i;

	if (snmpinitialized)
		return;	// if SNMP data structures already initialized 
	snmpinitialized = TRUE;
	hashinit();

	// initialize most SNMP variables 
    // SysUpTime = tod; done in clkint 
	SysDescr = vers;

	IfNumber = Net.nif - 1;
	
	IpDefaultTTL = IP_TTL;		IpInReceives = 0;
    IpInHdrErrors = 0;      IpInAddrErrors = 0;
	IpForwDatagrams = 0;		IpInUnknownProtos = 0;
	IpInDiscards = 0;		IpInDelivers = 0;
	IpOutRequests = 0;		IpOutDiscards = 0;
	IpOutNoRoutes = 0;		IpReasmTimeout = IP_FTTL;
	IpReasmReqds = 0;		IpReasmOKs = 0;
	IpReasmFails = 0;		IpFragOKs = 0;
	IpFragFails = 0;		IpFragCreates = 0;
	IcmpInMsgs = 0;			IcmpInErrors = 0;
	IcmpInDestUnreachs = 0;		IcmpInTimeExcds = 0;
	IcmpInParmProbs = 0;		IcmpInSrcQuenchs = 0;
	IcmpInRedirects = 0;		IcmpInEchos = 0;
	IcmpInEchoReps = 0;		IcmpInTimestamps = 0;
	IcmpInTimestampReps = 0;	IcmpInAddrMasks = 0;
	IcmpInAddrMaskReps = 0;		IcmpOutMsgs = 0;
	IcmpOutErrors = 0;		IcmpOutDestUnreachs = 0;
	IcmpOutTimeExcds = 0;		IcmpOutParmProbs = 0;
	IcmpOutSrcQuenchs = 0;		IcmpOutRedirects = 0;
	IcmpOutEchos = 0;		IcmpOutEchoReps = 0;
	IcmpOutTimestamps = 0;		IcmpOutTimestampReps = 0;
	IcmpOutAddrMasks = 0;		IcmpOutAddrMaskReps = 0;
	TcpRtoAlgorithm = 4;		TcpRtoMin = TCP_MINRXT*10;
	TcpRtoMax = TCP_MAXRXT*10;	TcpMaxConn = Ntcp;
	TcpActiveOpens = 0;		TcpPassiveOpens = 0;
	TcpAttemptFails = 0;		TcpEstabResets = 0;
	TcpCurrEstab = 0;		TcpInSegs = 0;
	TcpOutSegs = 0;			TcpRetransSegs= 0;
	UdpInDatagrams = 0;		UdpNoPorts = 0;
	UdpInErrors = 0;		UdpOutDatagrams = 0;

	for (i=0; i<Net.nif; ++i)
		nif[i].ni_lastchange = 0;
	*/
}
