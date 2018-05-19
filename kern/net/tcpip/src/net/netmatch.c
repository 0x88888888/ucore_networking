#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  netmatch  -  dst是否在网络 net中，mask是dst的子网掩码
 *------------------------------------------------------------------------
 */
int netmatch(IPaddr dst, IPaddr net, IPaddr mask, bool islocal) {
	
	if ((dst & mask) != (net & mask)){
		return false;
    }

	/*
	 * local srcs should not match broadcast addresses (host routes)
	 * islocal表示是否是本主机产生的数据
	 * 判断本机产生的这个数据是否是广播或者组播
	 *
	 *
	 */
	if (islocal)
		if (isbrc(dst) || IP_CLASSD(dst))
			return mask != ip_maskall;
	return true;
}

