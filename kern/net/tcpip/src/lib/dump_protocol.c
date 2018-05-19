#include <tcpip/h/network.h>

static void
dump_mac(char* pre, unsigned char* start, char* end) {
	cprintf("%s",pre);
	cprintf("%02x:%02x:%02x:%02x:%02x:%02x",
		start[0],
		start[1],
		start[2],
		start[3],
		start[4],
		start[5]);
    if (end != NULL) {
        cprintf("%s",end);
    }
}

static void
dump_ip(char* pre, IPaddr* ip, char* end) {
	cprintf("%s",pre);
	cprintf("%04d.%04d.%04d.%04d", 
		((*ip)&0xff000000)>>24,
		((*ip)&0x00ff0000)>>16,
		((*ip)&0x0000ff00)>>8,
		((*ip)&0x000000ff) );
    if (end != NULL  )  {
        cprintf("%s",end);
    }
}

static void 
dump_opt(char*pre ,char* opt_start, int len) {
    int i=0;
    cprintf("%s",pre);
    for(i=0; i < len; i++) {
       cprintf("%02x ", opt_start[i]);
    }
}

int 
dump_protocol(struct ep* pep) {
     struct ip*  pip;
     struct arp* parp;
     struct udp* pudp;
     struct tcp* ptcp;

     if ( ntohs(ep_type(pep)) !=  EPT_IP){
        return;
     } 
     pip = (struct ip*) pep->ep_data;
     if ( pip->ip_proto != IPT_UDP && pip->ip_proto != IPT_TCP) {
        return;
     }

      cprintf("  ETHER:\n");
     dump_mac("    dest mac: " ,ep_dst(pep), NULL);
     dump_mac("    src  mac: " ,ep_src(pep), "\n");
      cprintf("    eh_type: %x\n",ep_type(pep));
     
     if ( ntohs( ep_type(pep)) == EPT_ARP ) {
     	 parp = (struct arp*) pep->ep_data;
         cprintf("      ARP:\n");
         cprintf("        ar_hwtype: %02d\n", parp->ar_hwtype);
         cprintf("        ar_prtype: %02d\n", parp->ar_prtype);
         cprintf("        ar_hwlen: %02d\n", parp->ar_hwlen);
         cprintf("        ar_prlen: %02d\n", parp->ar_prlen);
         cprintf("        ar_op: %02d\n", parp->ar_op);
        dump_mac("        ar_sha: ", SHA(parp), NULL);
         dump_ip("        ar_spa: ", (IPaddr*)SPA(parp), "\n");
        dump_mac("        ar_tha: ", THA(parp), NULL);
         dump_ip("        ar_tpa: ", (IPaddr*)TPA(parp), "\n");

     } else if (ntohs(ep_type(pep)) == EPT_IP){
        pip= (struct ip*) pep->ep_data;
        cprintf("      IP:\n");
        cprintf("        IP_HLEN: %02d\n",IP_HLEN(pip));
        cprintf("        tos: %x \n", pip->ip_tos);
        cprintf("        ip_len(NO): %d \n", pip->ip_len);
        cprintf("        ip_len(HO): %d \n", ntohs(pip->ip_len));
        cprintf("        ip_id(NO): %d \n", pip->ip_id);
        cprintf("        ip_id(HO): %d \n", ntohs(pip->ip_id));
        cprintf("        ip_fragoff(HO): %x \n", pip->ip_fragoff);
        cprintf("        ip_fragoff(NO): %x \n", ntohs(pip->ip_fragoff));
        cprintf("        ip_ttl: %x \n", pip->ip_ttl);
        cprintf("        ip_proto: %d \n", pip->ip_proto);
        cprintf("        ip_cksum: %x \n", pip->ip_cksum);
        dump_ip("        ip_src: ",&(pip->ip_src), NULL);
        dump_ip("        ip_dst: ",&(pip->ip_dst), "\n");
        
        if (ntohs(pip->ip_proto)==IPT_UDP || pip->ip_proto == IPT_UDP) {
            pudp= (struct udp*)pip->ip_data;
        cprintf("          UDP:\n");
        cprintf("            u_src:  %d\n", pudp->u_src);
        cprintf("            host order u_src:  %d\n", ntohs(pudp->u_src));
        cprintf("            u_dst:  %d\n", pudp->u_dst);
        cprintf("            host order u_dst:  %d\n", ntohs(pudp->u_dst));
        cprintf("            u_len:  %d\n", pudp->u_len);
        cprintf("            host order u_len:  %d\n", ntohs(pudp->u_len));
        cprintf("            u_cksum:  %d\n", pudp->u_cksum);

        } else if(ntohs(pip->ip_proto == IPT_TCP) || pip->ip_proto == IPT_TCP ) {
            ptcp = (struct tcp*)pip->ip_data;
            cprintf("          TCP:\n");
            cprintf("            tcp_sport: %d\n",ptcp->tcp_sport);
            cprintf("            host order  tcp_sport: %d\n",ntohs(ptcp->tcp_sport));
        
            cprintf("            tcp_dport: %d\n",ptcp->tcp_dport);
            cprintf("            host order  tcp_dport: %d\n",ntohs(ptcp->tcp_dport));
        
            cprintf("            tcp_seq: %x \n",ptcp->tcp_seq);
            cprintf("            host order  tcp_seq: %x \n",ntohl(ptcp->tcp_seq));
        
            cprintf("            tcp_ack: %x \n",ptcp->tcp_ack);
            cprintf("            host order  tcp_ack: %x \n",ntohl(ptcp->tcp_ack));
        
            cprintf("            tcp_offset: %d \n",ptcp->tcp_offset);
            cprintf("            tcp_head len: %d \n",TCP_HLEN(ptcp));
        

            /*
                #define TCPF_URG    0x20
                #define TCPF_ACK    0x10
                #define TCPF_PSH    0x08
                #define TCPF_RST    0x04
                #define TCPF_SYN    0x02
                #define TCPF_FIN    0x01
            */
            cprintf("            tcp_code(flag):  \n",ptcp->tcp_code);
            cprintf("                      TCPF_ACK=%d\n",(ptcp->tcp_code & TCPF_ACK) ? 1:0 );
            cprintf("                      TCPF_PSH=%d\n",(ptcp->tcp_code & TCPF_PSH) ? 1:0 );
            cprintf("                      TCPF_RST=%d\n",(ptcp->tcp_code & TCPF_RST) ? 1:0 );
            cprintf("                      TCPF_SYN=%d\n",(ptcp->tcp_code & TCPF_SYN) ? 1:0);
            cprintf("                      TCPF_FIN=%d\n",(ptcp->tcp_code & TCPF_FIN) ? 1:0);
            cprintf("            tcp_window: %d \n",ptcp->tcp_window);
            cprintf("            host order tcp_window: %d \n",ntohs(ptcp->tcp_window));
            cprintf("            tcp_cksum: %x \n",ptcp->tcp_cksum);

            cprintf("            tcp_urgptr: %d \n",ptcp->tcp_urgptr);
            cprintf("            host order tcp_urgptr: %d \n",ntohs(ptcp->tcp_urgptr));
        
            dump_opt("           tcp options\n",((char*)ptcp) + 20, TCP_HLEN(ptcp) - 20);
            cprintf("\nptcp addr = %x\n", ptcp);
            cprintf("ptcp addr = %x\n",((char*)ptcp) + 20);
            if ( (ptcp->tcp_code & TCPF_ACK)){
                 static int cnt =0;
                 if (cnt < 2) {
                    cnt++;
                 }
             
            }

        } else {
            cprintf("ip unknown protocol ,net order: %d ,host order: %d\n",pip->ip_proto, ntohs(pip->ip_proto));
        }
     } else {
        cprintf(" ehter unknown protocol= %x\n", ntohs(ep_type(pep)));
     }     
      
    return 0;
}









