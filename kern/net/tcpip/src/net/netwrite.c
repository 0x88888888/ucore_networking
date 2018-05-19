#include <tcpip/h/network.h>
#include <vfs.h>

int32_t netwrite(struct netif* pni,struct ep* pep, int32_t len) {
	//struct inode* node=NULL;
	struct	arpentry 	*pae;
	int intr_flag;

    if (pni->ni_state != NIS_UP){
    	//网卡用的数据结构没有初始化好
    	freebuf(pep);
    	return SYSERR;
    }

    pep->ep_len = len;
#ifdef  DEBUG_NET
    if(pni != &nif[NI_LOCAL]) {
    	struct ip *pip = (struct ip*) pep->ep_data;
    	if (pip->ip_proto == IPT_OSPF) {
    		panic("ospf protocol ,not be implemented\n");
    	}
    }
#endif   // !DEBUG_NET 
    

    if (pni == &nif[NI_LOCAL]) {
        //进入回环处理
        return local_out(pep);
    } else if( isbrc(pep->ep_nexthop)) {
        //广播地址
    	memcpy(ep_dst(pep), pni->ni_hwb.ha_addr, EP_ALEN);
        net_dev_write(pni->ni_dev, pep, len);
        return OK;
        
    }
    /* else, look up the protocol address */
    
    local_intr_save(intr_flag);
    //从arp缓存中得到对应ip_dst对应的 ep_mac地址
    pae = arpfind( (unsigned char*)&pep->ep_nexthop, ep_type(pep), pni);
    
  
    if (pae && (pae->ae_state == AS_RESOLVED)) {
        //cprintf("sssddddddddddd\n");
        //panic("ssssssssssssssss");
        //得到目的ip主机的物理地址
    	memcpy(ep_dst(pep), pae->ae_hwa, pae->ae_hwlen);
    	local_intr_restore(intr_flag);
        //去发送数据就可以了
        

    	return net_dev_write(pni->ni_dev, pep, len);
    }

    if (IP_CLASSD(pep->ep_nexthop)) {
        //组播地址
    	local_intr_restore(intr_flag);
    	return SYSERR;
    }
    if(pae == 0) {
        // arp表中没有缓存，开始arp解析过程
    	pae =arpalloc();
        // pae->ae_state == AS_PENDING
    	pae->ae_hwtype = AR_HARDWARE;
    	pae->ae_prtype = EPT_IP;
    	pae->ae_hwlen = EP_ALEN;
    	pae->ae_prlen = IP_ALEN;
    	pae->ae_pni = pni;
    	if (pae->ae_queue != NULL) {
            freeq(pae->ae_queue);
    		//pae->ae_queue= create_queue(20);
    	} 
        //这里没的填
    	memcpy(pae->ae_pra, &pep->ep_nexthop, pae->ae_prlen);
    	pae->ae_attempts = 0;
    	pae->ae_ttl = ARP_RESEND;
    	arpsend(pae);
    }
    if (pae->ae_queue == NULL) {
    	pae->ae_queue= create_queue(ARP_QSIZE);
    }
    //将要发送的数据放到pae的待处理队列,arp解析结束的时候，处理
    if (enq(pae->ae_queue, pep)==false) {
    	freebuf(pep);
    }

    local_intr_restore(intr_flag);
    return OK;
}


