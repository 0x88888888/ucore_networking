#include <net.h>
#include <pci.h>
//#include <kmalloc.h>
#include <slab.h>
#include <sem.h>
#include <assert.h>
#include <stdio.h>
#include <mbox.h>
#include <mboxbuf.h>
#include <string.h>
#include <proc.h>
#include <tcpip/h/network.h>


const int max_data = 2048;
const int max_slots = 1024;

volatile uint32_t mbox_e1000_output;
volatile uint32_t mbox_e1000_input;
volatile uint32_t mbox_eth;
volatile uint32_t mbox_arp;
volatile uint32_t mbox_ip;
volatile uint32_t mbox_icmp;
volatile uint32_t mbox_udp;
volatile uint32_t mbox_tcp;

volatile uint32_t pid_e1000_output;
volatile uint32_t pid_e1000_input;
volatile uint32_t pid_ipout;


mutex  network_mtx;

void mbox_vars_init(){
   mbox_e1000_output = ipc_mbox_init(max_slots);
   mbox_e1000_input = ipc_mbox_init(max_slots);

   mbox_eth = ipc_mbox_init(max_slots);   
   mbox_arp = ipc_mbox_init(max_slots);
   mbox_ip  = ipc_mbox_init(max_slots);
   mbox_icmp = ipc_mbox_init(max_slots);
   mbox_udp = ipc_mbox_init(max_slots);
   mbox_tcp = ipc_mbox_init(max_slots);
   
}



// called from init proc (in init_main 在proc.c中)
void
start_net_mechanics() {
   
  
   mbox_vars_init();
   init_socket();
   
   mutex_init(&network_mtx);
   //mutex_init(&Net.sema);
   
   int pid=0;
   extern int e1000_input_proc(void*);
   extern int e1000_output_proc(void*);  
   extern int32_t netstart(void*); 

   lock(&network_mtx);
   pid = kernel_thread(netstart, NULL, 0);
   if (pid <= 0) {
      panic("create netstart failed.\n");
   }
   set_pid_name(pid, "netstart");
   

   lock(&network_mtx);
   cprintf("before kernel_thread e1000_input_proc\n");
   pid =kernel_thread(e1000_input_proc, NULL, 0);
   if (pid <= 0) {
      panic("create e1000_input_proc failed.\n");
   }
   set_pid_name(pid, "e1000_input_proc");
   pid_e1000_input = pid;
   cprintf("after kernel_thread e1000_input_proc\n");

   lock(&network_mtx); 
   cprintf("before kernel_thread e1000_output_proc\n");
   pid =kernel_thread(e1000_output_proc, 0, 0);
   if (pid <= 0) {
      panic("create e1000_out_proc failed.\n");
   }
   set_pid_name(pid, "e1000_output_proc");
   pid_e1000_output = pid;
   cprintf("after kernel_thread e1000_out_proc\n");


   //lock(&network_mtx);

}


void net_init() {
   pci_init();
   
}