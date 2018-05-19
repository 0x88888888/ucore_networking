#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 *  ipfhcopy -  copy the hardware, IP header, and options for a fragment
 *------------------------------------------------------------------------
 */
int ipfhcopy(struct	ep* pepto,struct ep* pepfrom, unsigned int offindg){
    
    struct ip* pipfrom = (struct ip*)pepfrom->ep_data;
    unsigned int i, maxhlen, olen, otype;
    unsigned int hlen = (IP_MINHLEN<<2);

    if (offindg == 0) {
        //第一个分片
    	blkcopy(pepto, pepfrom, EP_HLEN + IP_HLEN(pipfrom));
    	return IP_HLEN(pipfrom);
    }

    //ep和ip的头部基本部分，不包括ip option
    blkcopy(pepto, pepfrom, EP_HLEN+hlen);
    /* copy options */
    maxhlen = IP_HLEN(pipfrom);
    i = hlen;
    while(i < maxhlen) { //有option数据
    	otype = pepfrom->ep_data[i]; /*option数据类型*/
    	olen = pepfrom->ep_data[++i];/*option数据长度*/
    	if (otype & IPO_COPY){
    	   blkcopy(&pepto->ep_data[hlen], &pepfrom->ep_data[i-1], olen);
    	   hlen += olen;
        } else if (otype == IPO_NOP || otype == IPO_EOOP) {
           pepto->ep_data[hlen++] = otype;
           olen = 1;
        }
    }
    /* pad to a multiple of 4 octets */
    while(hlen % 4)
    	pepto->ep_data[hlen++] = IPO_NOP;
    return hlen;
}


