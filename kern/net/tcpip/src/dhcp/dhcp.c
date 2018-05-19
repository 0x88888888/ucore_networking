#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * dhcp_get_opt_val  -	Retrieve a pointer to the value for a specified
 *			  DHCP options key
 *------------------------------------------------------------------------
 */
char* 
dhcp_get_opt_val(const struct dhcpmsg* dmsg, uint32_t dmsg_size, uint8_t option_key) {
    unsigned char*  opt_tmp;
    unsigned char*  eom;

    eom = (unsigned char*)dmsg + dmsg_size - 1;
    opt_tmp = (unsigned char*)dmsg->dc_opt;

    while(opt_tmp < eom) {
    	/* If the option value matches return the value */
    	if ((*opt_tmp) == option_key) {
    		/* Offset past the option value and size */
    		return (char*)(opt_tmp + 2);
    	}
    	opt_tmp++; /* Move to length octet */
    	opt_tmp +=*(uint8_t*)opt_tmp + 1;
    }

    /* Option value not found */
    return NULL;
}

static void
printdhcpmsg(char* pre,struct dhcpmsg *pdhcp) {
    int i=0; 
    cprintf("%s\n",pre);
    cprintf("------------------------------------------\n");
    cprintf("dc_bop= %02x \n", pdhcp->dc_bop);
    cprintf("dc_htype= %02x \n",pdhcp->dc_htype);
    cprintf("dc_hlen= %02x \n",pdhcp->dc_hlen);
    cprintf("dc_hops= %02x \n",pdhcp->dc_hops);
    cprintf("dc_xid= %x \n",pdhcp->dc_xid);
    cprintf("dc_secs= %x \n",pdhcp->dc_secs);
    cprintf("dc_flags= %x \n",pdhcp->dc_flags);
    cprintf("dc_cip= %x \n",pdhcp->dc_cip);
    cprintf("dc_yip= %x \n",pdhcp->dc_yip);
    cprintf("dc_sip= %x \n",pdhcp->dc_sip);
    cprintf("dc_gip= %x \n",pdhcp->dc_gip);
    cprintf("dc_chaddr:\n");
    for(i=0;i <16; i++) {
       cprintf("%02x ",pdhcp->dc_chaddr[i]);
    }
    cprintf("\n------------------------------------------\n");
    cprintf("\n");
}

/*------------------------------------------------------------------------
 * dhcp_bld_bootp_msg  -  Set the common fields for all DHCP messages
 *------------------------------------------------------------------------
 */
void
dhcp_bld_bootp_msg(struct dhcpmsg* dmsg, int nif_num) {
	uint32_t  xid = 0;     /* xid used for the exchage */
    struct netif* pnif=&nif[nif_num];

	memcpy(&xid, pnif->ni_hwa.ha_addr, 4); /* Use 4 bytes from mac as unique XID */

	memset(dmsg, 0x00, sizeof(struct dhcpmsg));

	dmsg->dc_bop   = 0x01;   /* Outgoing request  */
	dmsg->dc_htype = 0x01; /* Hardware type is Ethernet */
	dmsg->dc_hlen  = 0x06;  /* Hardware address length   */
	dmsg->dc_hops  = 0x00;  /* Hop count                 */
    dmsg->dc_xid   = htonl(xid);
	dmsg->dc_secs  = 0x0000; /* Seconds                  */
	dmsg->dc_flags = 0x0000; /* Flags                   */
	dmsg->dc_cip = 0x00000000; /* Client IP address     */
	dmsg->dc_yip = 0x00000000; /* Your IP address       */
	dmsg->dc_sip = 0x00000000; /* Server IP address     */
	dmsg->dc_gip = 0x00000000; /* Gateway IP address    */
	memset(&dmsg->dc_chaddr, '\0', 16); /* Client hardware address */
    memcpy(&dmsg->dc_chaddr, pnif->ni_hwa.ha_addr, EP_ALEN);
    memset(&dmsg->dc_bootp, '\0', 192); /* Zero the boot area */
    dmsg->dc_cookie = htonl(0x63825363);/* Magic cookie for DHCP	*/
}

/*------------------------------------------------------------------------
 * dhcp_bld_disc  -  handcraft a DHCP Discover message in dmsg
 *
 * 构建 dhcp服务dsicover报文
 *------------------------------------------------------------------------
 */
int32_t
dhcp_bld_disc(struct dhcpmsg* dmsg, int nif_num)
{
	uint32_t  j = 0;

	dhcp_bld_bootp_msg(dmsg, nif_num);
    //236 +4 + 10 = 246
	dmsg->dc_opt[j++] = 0xff & 53;	/* DHCP message type option	*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* Option length		*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* DHCP Dicover message		*/
	dmsg->dc_opt[j++] = 0xff &  0;	/* Options padding		*/

	dmsg->dc_opt[j++] = 0xff & 55;	/* DHCP parameter request list	*/
	dmsg->dc_opt[j++] = 0xff &  2;	/* Option length		*/
	dmsg->dc_opt[j++] = 0xff &  1;	/* Request subnet mask 		*/
	dmsg->dc_opt[j++] = 0xff &  3;	/* Request default router addr->*/
	
	dmsg->dc_opt[j++] = 0xff;	/* End of options		*/
    
    //dhcpmsg有效数据长度
	return (uint32_t)((char *)&dmsg->dc_opt[j] - (char *)dmsg + 1);
}

/*------------------------------------------------------------------------
 * dhcp_bld_req - handcraft a DHCP request message in dmsg
 *
 * 构建 dhcp ip地址请求报文
 *------------------------------------------------------------------------
 */
int32_t dhcp_bld_req(
  struct dhcpmsg* dmsg,		/* DHCP message to build	*/
  const struct dhcpmsg* dmsg_offer, /* DHCP offer message	*/
  uint32_t dsmg_offer_size,	/* Size of DHCP offer message	*/
  int32_t nif_num
) 
{
    uint32_t j = 0;
    uint32_t* server_ip; /* Take the DHCP server IP addr */
                         /* from dhcp offer message */

    dhcp_bld_bootp_msg(dmsg, nif_num);
    dmsg->dc_sip = dmsg_offer->dc_sip; /* Server IP address		*/
    dmsg->dc_opt[j++] = 0xff & 53;	/* DHCP message type option	*/
    dmsg->dc_opt[j++] = 0xff &  1;	/* Option length		*/
    dmsg->dc_opt[j++] = 0xff &  3;	/* DHCP Request message		*/
    dmsg->dc_opt[j++] = 0xff &  0;	/* Options padding		*/

    dmsg->dc_opt[j++] = 0xff & 50;	/* Requested IP			*/
    dmsg->dc_opt[j++] = 0xff &  4;	/* Option length		*/
    memcpy(((void *)&dmsg->dc_opt[j]), &dmsg_offer->dc_yip, 4);
    j += 4;
    
    /* Retrieve the DHCP server IP from the DHCP options */
    server_ip = (uint32_t*) dhcp_get_opt_val(dmsg_offer,
    	                   dsmg_offer_size,
    	                   DHCP_SERVER_ID);
    
    if(server_ip == 0) {
        cprintf("Unable to get server IP add. from DHCP Offer\n");
        return SYSERR;
    }

    dmsg->dc_opt[j++] = 0xff & 54; /* server IP */
    dmsg->dc_opt[j++] = 0xff & 4;  /* Option length */
    memcpy((void *)&dmsg->dc_opt[j], server_ip, 4);
    j += 4;

    dmsg->dc_opt[j++] = 0xff;     /* End of options */

    return (uint32_t)((char *)&dmsg->dc_opt[j] - (char *)dmsg + 1);
}

/*------------------------------------------------------------------------
 * getlocalip - use DHCP to obtain an IP address
 *------------------------------------------------------------------------
 */
uint32_t
getlocalip(int nif_num) {
   
    struct   dhcpmsg* pdmsg_snd;  /* Holds outgoing DHCP message */
    struct   dhcpmsg* pdmsg_rvc;  /* Holds incoming DHCP message */
    
    //dmsg_snd = (struct dhcpmsg*)ptrxgram->xg_data;

    struct netif* pnif=&nif[nif_num];
    int32_t i, j;        /* Retry counters		 */
    int32_t len;         /* Length of data sent */

    int32_t inlen;       /* Length of data received */
    char*   optptr;      /* Pointer to options area */
    char*   eop;         /* Address of end of packet */
    int32_t msgtype;     /* Type of DCHP message */

    uint32_t addrmask;   /* Address mask for network */
    uint32_t routeraddr; /* Default router address */
    uint32_t dnsaddr;    /* DNS server address     */
    uint32_t ntpaddr;    /* NTP server address     */
    
    uint32_t tmp;      
    
    struct ep*  pepsnd = (struct ep*)kmalloc(sizeof(struct ep));
    struct ip*  pipsnd = (struct ip*)pepsnd->ep_data;
    bzero( pepsnd, sizeof(struct ep));

    struct udp* pudpsnd = (struct udp*)pipsnd->ip_data;
    pdmsg_snd  = (struct dhcpmsg*) pudpsnd->u_data;;


    struct ep*  peprcv = (struct ep*)kmalloc(sizeof(struct ep));
    bzero( peprcv, sizeof(struct ep));
    struct ip*  piprcv = NULL;
    struct udp* pudprcv = NULL;

    
    struct device* pdev =&devtab[nif[nif_num].ni_dev];
    //pdev->dev_e1000_write(pdev,buff, len)
    struct etblk* petblk = (struct etblk*)pdev->dvioblk;


	
    //discover 数据包
	len = dhcp_bld_disc(pdmsg_snd, nif_num);
    cprintf("len = %d\n", len);
	if (len == SYSERR) {
	   cprintf("getlocalip: Unable to build DHCP discover\n");
	   return SYSERR;
	}
    pipsnd->ip_src = 0x00;
    pipsnd->ip_dst = 0xffffffff;

    pudpsnd->u_src = UDP_DHCP_CPORT;
    pudpsnd->u_dst = UDP_DHCP_SPORT;
    pudpsnd->u_cksum = 0;

    for(i = 0; i< DHCP_RETRY; i++) {
       
       memcpy(ep_dst(pepsnd), petblk->etbcast, EP_ALEN); 
       bzero(ep_src(pepsnd),EP_ALEN);
       ep_type(pepsnd) = EPT_IP; 

       pipsnd->ip_verlen=(IP_VERSION<<4) | IP_MINHLEN;
       pipsnd->ip_tos = 0;
       pipsnd->ip_id = getnext_ip_id();
       pipsnd->ip_fragoff = 0;
       pipsnd->ip_ttl = 0xff;
       pipsnd->ip_proto= IPT_UDP;
       pipsnd->ip_cksum = 0;
       pipsnd->ip_len =  (IP_MINHLEN * 4) + U_HLEN + len;//ip 长度
       
       pudpsnd->u_src = UDP_DHCP_CPORT;
       pudpsnd->u_dst = UDP_DHCP_SPORT;
       pudpsnd->u_cksum = 0;
       pudpsnd->u_len =  U_HLEN + len;

       udph2net(pudpsnd);
       pudpsnd->u_cksum =0;
   
       
       iph2net(pipsnd);
       pipsnd->ip_cksum=cksum(pipsnd, IP_HLEN(pipsnd)/2);

  
       pdev->d_write(pdev, pepsnd, EXTRAEPSIZ+ EH_LEN + IP_HLEN(pipsnd) + U_HLEN + len);
       
        //开始读
        for ( j=0 ; j< 3; j++) {
            do_sleep(50);// 0.5秒之后去读
            inlen=pdev->d_read(pdev, peprcv, sizeof(struct ep));
            if (inlen <= 0) {
                continue;
            }

            piprcv = (struct ip*) peprcv->ep_data;
            if (piprcv->ip_proto != IPT_UDP) {
                continue;
            }

            pudprcv = (struct udp*) ((char*)piprcv + IP_HLEN(piprcv));
            
            pdmsg_rvc =(struct dhcpmsg*) ((char*)pudprcv + U_HLEN);
            
            if (pdmsg_rvc->dc_xid != pdmsg_snd->dc_xid) {
                continue;
            }
            ipnet2h(piprcv);
          
            //dhcp数据长度
            inlen = piprcv->ip_len - IP_HLEN(piprcv) - U_HLEN; 
            eop = (char*)pdmsg_rvc + inlen - 1;//dhcp数据终点
            optptr =(char*)pdmsg_rvc->dc_opt;
          
            //在服务端发过来的offer包中包含下面的这些信息，先保持
            while (optptr < eop) {

                switch (*optptr) {
                    case 53:    // Message type 
                        msgtype = 0xff & *(optptr+2);
                        break;

                    case 1:     // Subnet mask 
                        memcpy((void *)&tmp, optptr+2, 4);
                        addrmask = ntohl(tmp);
                        break;

                    case 3:     // Router address 
                        memcpy((void *)&tmp, optptr+2, 4);
                        routeraddr = ntohl(tmp);
                        break;

                    case 6:     // DNS server address 
                        memcpy((void *)&tmp, optptr+2, 4);
                        dnsaddr = ntohl(tmp);
                        break;

                    case 42:    // NTP server address 
                        memcpy((void *)&tmp, optptr+2, 4);
                        ntpaddr = ntohl(tmp);
                        break;

                }
                optptr++;   // Move to length octet 
                optptr += (0xff & *optptr) + 1;
            }

            if (msgtype == 0x02) { // Offer - send request  
                //request这个offer
                len = dhcp_bld_req(pdmsg_snd, pdmsg_rvc, inlen, nif_num);
                if(len == SYSERR) {
                    cprintf("getlocalip: %s\n",
                     "Unable to build DHCP request");
                  return SYSERR;
                }
                pdev->d_write(pdev, pepsnd, EXTRAEPSIZ + EH_LEN + IP_MINHLEN + U_HLEN + len);
                continue;
            } else if (pdmsg_rvc->dc_opt[2] != 0x05) {
                //dhcp server 发过来的非 ack包，跳过去
                // If not an ack skip it 
                continue;
            }

            //dhcp server回复ack包，保存前面offer包中的信息到nif，然后退出
            if (addrmask != 0) {
                pnif->ni_mask = addrmask;
            }
            
            if (routeraddr != 0) {
                pnif->ni_gateway = routeraddr;
            }
            
            if (dnsaddr !=0) {
                pnif->ni_nserver = dnsaddr;
            }
            if (ntpaddr !=0){
                pnif->ni_tserver = ntpaddr;
            } 
          
            //ip地址
            pnif->ni_ip= ntohl(pdmsg_rvc->dc_yip);
            
            //广播地址
            pnif->ni_brc = (pnif->ni_ip & pnif->ni_mask) | (~pnif->ni_mask);
            pnif->ni_ivalid = true;
            
            rtadd(pnif->ni_brc, pnif->ni_mask, routeraddr, 1, nif_num, RT_INF);
            return pnif->ni_ip;
       }
    }
    
    panic("dhcp exit 1");
  
    return SYSERR;
}







