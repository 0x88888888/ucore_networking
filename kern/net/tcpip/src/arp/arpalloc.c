#include <tcpip/h/network.h>

/*------------------------------------------------------------------------
 * arpalloc - 分配一个 arpentry
 *------------------------------------------------------------------------
 */
struct arpentry* arpalloc() {
   static int aenext = 0;
   struct arpentry *pae; 
   int i;
   for (i=0; i<ARP_TSIZE; i++) {
        if (arptable[aenext].ae_state == AS_FREE)
        	break;
        aenext = (aenext + 1) % ARP_TSIZE;
   }
   pae = &arptable[aenext];
   aenext = (aenext + 1) % ARP_TSIZE;
   
   //
   if ( (pae->ae_state == AS_PENDING) &&  
   	    (NULL !=pae->ae_queue) ) {
   	 arpdq(pae);
   }
   pae->ae_state = AS_PENDING;
   pae->ae_ttl = 1;
   return pae;
}
