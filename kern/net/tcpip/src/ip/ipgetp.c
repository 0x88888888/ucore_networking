#include <tcpip/h/network.h>

static  int ifnext = NI_LOCAL;

/*------------------------------------------------------------------------
 * ipgetp  --  choose next IP input queue and extract a packet
 *------------------------------------------------------------------------
 */
struct ep*
ipgetp(int* pifnum)
{
  struct ep *pep;
  int i,msg=0,senderpid=0,event=-1;
 
  while(true) {
    //从各个nif->ni_ipinq中获取ip包，用于处理，传到上层或者传到下层
  	for (i=0; i<Net.nif; ++i,++ifnext) {
        if (ifnext >= Net.nif)
            ifnext = 0;
        if (nif[ifnext].ni_state == NIS_DOWN)
            continue;
          //NIGET中 ni_ipinq中摘取数据
        if ( (pep = NIGET(ifnext))) {
        	*pifnum = ifnext;
        	return pep;
        }
  	}
    //如果所有的nif没有数据，等待数据过来，event为发送ifnum
    msg=ipc_event_recv(&senderpid,EVT_TYP_IP_EVENT , &event, 2000);

    if (msg !=-E_INVAL && msg != -E_TIMEOUT) {
      ifnext=event;
    }
  }
  return NULL;
}

