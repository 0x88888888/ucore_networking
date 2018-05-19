#include <tcpip/h/network.h>

int	ippid, gateway, bsdbrc;

/*------------------------------------------------------------------------
 *  ipproc  
 *
 *  
 *------------------------------------------------------------------------
 */
 int32_t ipproc(void* args) {
    struct ep* pep;
    struct ip* pip;
    struct route *prt;
    bool nonlocal; //标记ip数据是否来自外部，并转发到外部接口
    int ifnum = -1; 

 	ippid = getpid(); 

    unlock(&Net.sema);

    while(true) {
    	//循环nif,一直接收数据，直到有数据
        pep = ipgetp(&ifnum); 
        cprintf("ipproc get ep from ifnum = %d \n",ifnum);
        pip = (struct ip*)pep->ep_data;
        if ((pip->ip_verlen >>4) != IP_VERSION) {
        	cprintf("ip_verlen error\n");
        	//ip版本不对
        	IpInHdrErrors++;
			freebuf(pep);
			continue;
        }
        if ( IP_CLASSE(pip->ip_dst)) {
        	//E类地址，不在本进程处理范围内
            cprintf("IP_CLASSE error\n");
			IpInAddrErrors++;
			freebuf(pep);
			continue;
		}

		if (ifnum != NI_LOCAL) {
			/* 
			 * 上层 udp,ip协议产生的数据通过 ipsend函数发过来， ip头部还是主机序
			 * 不是本机产生的数据，是由外部借口进入的数据
			 */
			if(cksum((unsigned short*)pip, IP_HLEN(pip)>>1)) {
				IpInHdrErrors++;
				freebuf(pep);
				continue;
			}

			ipnet2h(pip);
		}
        
        /*
         * 代码到此之前 ip层头部，不确定是网络序还是主机序
         * 在此之后 ip层头部都是主机序
         */

		//获取该报文的路由信息

        prt = rtget(pip->ip_dst, (ifnum == NI_LOCAL));

        if (prt == NULL) {
			if (gateway) {
				//作为网关时
				iph2net(pip);
				//网络不可达
				icmp(ICT_DESTUR, ICC_NETUR,
						pip->ip_src, pep, 0);
			} else {
				//不是网关时，丢弃
				IpOutNoRoutes++;
				freebuf(pep);
			}
			continue;
		}
		//是否来自外部主机
		/*
		 * ifnum != NI_LOCAL 说明数据是由nic接受进来的，数据来自外部
		 * prt->rt_ifnum != NI_LOCAL 说明本机不是数据到达的目的地 ，
		 *             这种情况下，如果主机不是作为 gateway，那就不对了，因为要转发数据
		 *
		*/        
		nonlocal = ifnum != NI_LOCAL && prt->rt_ifnum != NI_LOCAL;
		if(!gateway && nonlocal) {
			//本机不做网关，数据的接口又非 NI_LOCAL，所以本机是数据的目的主机了
			//不能转发，但是又是外部的数据,本机处理
            if(pip->ip_proto == IPT_UDP) {
                //udp协议数据
                udp_in(&nif[ifnum], pep);
            } else if (pip->ip_proto == IPT_TCP) {
                //tcp协议数据
                tcp_in(&nif[ifnum], pep);
            } else if (pip->ip_proto == IPT_ICMP) {
            	//icmp数据
                icmp_in(&nif[ifnum], pep);
            } else if (pip->ip_proto == IPT_IGMP) {
            	//igmp数据
            	panic("ipproc: igmp protocol not be implemented\n");
            } else if (pip->ip_proto == IPT_EGP) {
                panic("ipproc: egp protocol not be implemented\n");
            } else if (pip->ip_proto == IPT_OSPF) {
                panic("ipproc: ospf protocol not be implemented\n");
            } else {
            	//未知协议
                panic("ipproc: unknown protocol : %d \n",pip->ip_proto);
                IpInAddrErrors++;
                freebuf(pep);
                rtfree(prt);
            }

			//IpInAddrErrors++;
			//freebuf(pep);
			//rtfree(prt);
			continue;
		}
		if (nonlocal){
			IpForwDatagrams++;
		}
         
        //到此就需要转发或者发送ip数据包到网络
		
		if (ifnum == NI_LOCAL) {
			/* fill in src IP, if we're the sender */
			//数据报由本主机生成 , 填写源ip
			if (pip->ip_src == ip_anyaddr){
				//没有填写源ip，填写ip_src
				if(prt->rt_ifnum == NI_LOCAL) {
					//本机发送到本机
					pip->ip_src = pip->ip_dst;
				}else {
					//通过本机的某个网卡，发送到外部去
					pip->ip_src = nif[prt->rt_ifnum].ni_ip;
				}
			}
		}

		if (--(pip->ip_ttl)==0 && prt->rt_ifnum != NI_LOCAL) {
			// 作为网关转发，但是不可达
			//ttl==0，返回icmp超时
			IpInHdrErrors++;
			iph2net(pip);
			icmp(ICT_TIMEX, ICC_TIMEX, pip->ip_src, pep, 0);
			rtfree(prt);
			continue;
		}
		//是否定向广播处理
		ipdbc(ifnum, pep, prt);  /* handle directed broadcasts */
		//是否icmp重定向处理
		ipredirect(pep, ifnum, prt); /* do redirect, if needed	*/
        //
		if (prt->rt_metric != 0) {
			//转发到路由网关地址 ifnum==1
			cprintf("before ipputp ifnum=%d nexthop=%x\n", prt->rt_ifnum, prt->rt_gw);
			ipputp(prt->rt_ifnum, prt->rt_gw, pep);
		}else {
			cprintf("before ipputp ifnum=%d dst=%x\n", prt->rt_ifnum, pip->ip_dst);
			ipputp(prt->rt_ifnum, pip->ip_dst, pep);
		}
		rtfree(prt);
    }
 	return OK;
 }
