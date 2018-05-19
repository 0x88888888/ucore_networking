#include <tcpip/h/network.h>

/* ECHOMAX must be an even number */
#define ECHOMAX(pip)    (MAXLRGBUF-IC_HLEN-IP_HLEN(pip)-EP_HLEN)

/*------------------------------------------------------------------------
 *  icsetdata -  set the data section. Return value is data length
 *------------------------------------------------------------------------
 */
int icsetdata(int type, struct ip *pip, char *pa2) {
	struct icmp *pic = (struct icmp *)pip->ip_data;
	int i,len;

	switch(type) {
		case ICT_ECHORP:
		     len = pip->ip_len - IP_HLEN(pip) - IC_HLEN;
		     if (isodd(len)) {
		     	pic->ic_data[len] = 0; /* so cksum works */
		     }
		     return len;
		case ICT_DESTUR:
		case ICT_SRCQ:
		case ICT_TIMEX:
		     pic->ic_mbz = (long)0;
		     break;
		case ICT_REDIRECT:
		     // redirect 设置 新路由信息
		     //blkcopy(pic->ic_gw, pa2, IP_ALEN);
		     pic->ic_gw = (IPaddr)pa2;
		     break;
		case ICT_PARAMP:
             len = (int)pa2;
             pic->ic_ptr = (char)len;
             for (i=0; i<IC_PADLEN; ++i)
                 pic->ic_pad[i] = 0;
             break;
        case ICT_MASKRP:
             // 网络掩码请求回复
             blkcopy(pic->ic_data, pa2, IP_ALEN);
             break;
        case ICT_ECHORQ:
             //pa2==56
             if ((unsigned)pa2 > (unsigned)(ECHOMAX(pip)))
                pa2 = (char *)(ECHOMAX(pip));
             for (i=0; i<(int)pa2; ++i)
                pic->ic_data[i] = i;//填空，废物数据
             if (isodd(pa2))
                pic->ic_data[(int)pa2] = 0;
             return (int)pa2;
        case ICT_MASKRQ:
             blkcopy(pic->ic_data, &ip_anyaddr, IP_ALEN);
             return IP_ALEN;
	}
	return 0;
}


