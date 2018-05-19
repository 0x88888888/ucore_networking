#include <tcpip/h/network.h>

struct ep* icsetbuf();
void  icsetsrc();


/*------------------------------------------------------------------------
 *  icmp -  发送icmp信息
 * 
 * type 为不同的取值时, pa1 ,pa2的意义
 * ICT_REDIRECT - pa2 == gateway address
 * ICT_PARAMP   - pa2 == (packet) pointer to parameter error
 * ICT_MASKRP   - pa2 == mask address
 * ICT_ECHORQ   - pa1 == seq, pa2 == data size
 *------------------------------------------------------------------------
 */
//int32_t icmp(int type, int code, IPaddr dst, char* pa1, char* pa2)
int32_t icmp(int type, int code, IPaddr dst, void* pa1, void* pa2) 
{
    struct ep    *pep;
    struct ip    *pip;
    struct icmp  *pic;
    bool isresp; //是否是应答报文
    bool iserr;  //是否是差错报文
    IPaddr tdst;
    int datalen;
    tdst = dst; /* worry free pass by value	*/
   
    pep = icsetbuf(type, pa1, &isresp, &iserr);

    if (pep == NULL) {
    	IcmpOutErrors++;
    	return SYSERR;
    }
    pip = (struct ip*)pep->ep_data;
    pic = (struct icmp*)pip->ip_data;

    datalen = IC_HLEN;

    /* we fill in the source here, so routing won't break it */

    if (isresp) {
        //是应答报文
    	if (iserr) {
    		if(!icerrok(pep)) {
    			freebuf(pep);
    			return OK;
    		}
            //允许发送该差错报告
            //将ip首部和8字节的ip数据复制到icmp数据中去
    		blkcopy(pic->ic_data, pip, IP_HLEN(pip)+8);
    		datalen += IP_HLEN(pip) + 8;
    	}
    	icsetsrc(pip);
    } else {
        //不是应答报文，发送方ip: 0.0.0.0
    	pip->ip_src = ip_anyaddr;
    }
    pip->ip_dst = tdst;

    pic->ic_type = (char) type;
    pic->ic_code = (char) code;
    if (!isresp) {
        if (type == ICT_ECHORQ) {
            // request 序号
            pic->ic_seq = (int) pa1;
        } else {
            pic->ic_seq = 0;
        }
        //不是reply,就是request,request别人，需要记录pid，等待别人的reply
        pic->ic_id = getpid();
    }

    datalen += icsetdata(type, pip, pa2);

    pic->ic_cksum =0;
    pic->ic_cksum =cksum((unsigned short*)pic, (datalen+1)>>1);
    //cprintf("before icmp send\n");
    pip->ip_proto = IPT_ICMP;  /* for generated packets */
    ipsend(tdst, pep, datalen, IPT_ICMP, IPP_INCTL, IP_TTL);
    return OK;
} 


