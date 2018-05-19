#include <tcpip/h/network.h>

//dns解析
int32_t resolve(char* nam) {
	panic("name2ip.c resolve not be implemented\n");
}

//dns解析
IPaddr
name2ip(char *nam)
{
	bool	isnum;
	char	*p;
	IPaddr  ip;

	isnum = true;
	for (p=nam; *p; ++p) {
		isnum &= ((*p >= '0' && *p <= '9') || *p == '.');
		if (!isnum)
			break;
	}
	if (isnum){
		 dot2ip(nam,&ip);
		 return ip;
		}
	return resolve(nam);
}


