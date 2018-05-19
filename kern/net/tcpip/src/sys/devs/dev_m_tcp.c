#include <tcpip/h/network.h>

int
tcp_m_open(struct device *dev, uint32_t open_flags, uint32_t arg2) {
    
    return tcpmopen(dev, (void *)open_flags, (void *)arg2);
    //return OK;
}

int
tcp_m_iocntl(struct device *dev, int op, void* arg1, void* arg2) {
	return tcpmcntl(dev, op,  arg1);
	//return OK;
}

