#include <proc.h>
#include <sched.h>
#include <sync.h>
#include <e1000.h>
#include <sem.h>
#include <stdio.h>

#include <tcpip/h/network.h>
#include <net.h>
int e1000_input_proc(void* arg) {

	int recvlen=0;
	//int idx =0;
	int eplen=sizeof(struct ep);
    bool intr_flag;
    
    unlock(&network_mtx);
    cprintf("before e1000_input_proc loop start\n");
    uint8_t* bufrecv=(uint8_t*) kmalloc(sizeof(struct ep));
    memset(bufrecv, 0 , eplen);


	while (true) {
		recvlen=0;


	    schedule();
   
   	    local_intr_save(intr_flag);
	    recvlen=e1000_recv(bufrecv, eplen);

	    if (recvlen > 0) {
	    	struct ep* pep = (struct ep*)kmalloc(recvlen); //(struct ep*)bufrecv;
	    	
	    	memcpy(pep, bufrecv, recvlen);
	    	cprintf("\n\ne1000_input_proc  kmalloc pep=%x\n",pep);
	    	dump_protocol(pep);
	    	cprintf("------------------------------- =%x\n\n",pep);

	    	ni_in(&nif[NI_PRIMARY],pep, recvlen);

	    	memset(bufrecv, 0 , eplen);
	    }
	    local_intr_restore(intr_flag);
	    //del_timer(timer);
	}
	
    return 0;
}






