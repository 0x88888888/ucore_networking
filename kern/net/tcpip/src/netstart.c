#include <tcpip/h/network.h>
#include <net.h>

extern  int	bsdbrc;
extern  int gateway;

struct  netinfo    Net;
struct  netif      nif[NIF];

int32_t netstart(void* args) {
      //char str[128] ={0};
      IPaddr temp;
      IPaddr defgateway;
      int i=0;
      int pid;
      
                   //UPPS==Ndg==16
      for(i =0; i< UPPS;i++) {
         upqs[i].up_valid=false;
      }

      for(i=0; i<NDEVS; i++) {
            init(i);//初始化devtab中的设备对象,devtab[i]->d_init(&dev);
      }

      portinit(MAXMSGS);
      
      
      mutex_init(&udpmutex);

      arpinit();
      ipfinit();  /* init the IP frag queue */
      

      gateway = 0;

      bsdbrc =1;

      if (gateway) {
      	panic("the machin should not be gateway\n");
      } else {
            /*
             * inithost的主要工作是设置网卡硬件地址
             * 设置相应的nif为 ip_anyaddr
             */
      	inithost();
      }
      
      

      rtadd(RT_LOOPBACK, ip_maskall, RT_LOOPBACK,0 , NI_LOCAL, RT_INF);

      //IPaddr ip= dot2ip(DEFIP);
      dot2ip(DEFGATEWAY,&defgateway);
      rtadd(RT_DEFAULT, ip_maskall, defgateway, RTM_INF-1 ,1, RT_INF);
      


      cprintf("Net.nif =%d \n",Net.nif);
      for(i = 0 ; i<Net.nif; i++) {
         if(nif[i].ni_ipinq ==NULL){
            nif[i].ni_ipinq=create_queue(20);
         }
         nif[i].ni_hwtype = AR_HARDWARE;   /*for arp*/

         if (i< 2) {
            nif[i].ni_state = NIS_UP;
         }
      }

   
      for(i=1;i<Net.nif; i++) {
            //dhcp获取网卡的ip地址
            if (nif[i].ni_state !=NIS_UP){
                  continue;
            }

            temp= getlocalip(i);
            cprintf("getaddr result: %02x:%02x:%02x:%02x \n",
                  (temp&0xff000000)>>24,
                  (temp&0x00ff0000)>>16, 
                  (temp&0x0000ff00)>>8,
                  (temp&0x000000ff));
            

            //rtadd(nif[i].ni_ip, ip_maskall, nif[i].ni_ip, 0,NI_LOCAL, RT_INF);
      }
      
     
      lock(&Net.sema);
      pid=kernel_thread(slowtimer_proc, NULL, 0);
      set_pid_name(pid, "slowtimer");

      lock(&Net.sema);
      pid=kernel_thread(ipproc, NULL, 0);
      set_pid_name(pid ,"ipproc");
      
      lock(&Net.sema);
      pid=kernel_thread(tcptimer, NULL, 0);
      set_pid_name(pid,"tcptimer");

      lock(&Net.sema);
      pid=kernel_thread(tcpinp, NULL, 0);
      set_pid_name(pid, "tcpinp");
     
      
      lock(&Net.sema);
      pid=kernel_thread(tcpout, NULL, 0);
      set_pid_name(pid, "tcpout");
      
      lock(&Net.sema);
      pid=kernel_thread(tcpinp, NULL, 0);
      set_pid_name(pid, "tcpinp");

      
      
      lock(&Net.sema);
      pid=kernel_thread(rip, NULL, 0);
      set_pid_name(pid, "ripin");

      lock(&Net.sema);
      pid=kernel_thread(udpechod, NULL, 0);
      set_pid_name(pid, "udpechod");
      
      lock(&Net.sema);
      pid=kernel_thread(tcpechod, NULL, 0);
      set_pid_name(pid, "tcpechod");
      
      unlock(&network_mtx);
      return OK;
}


