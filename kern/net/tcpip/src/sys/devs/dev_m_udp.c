#include <tcpip/h/network.h>

int
udp_m_open(struct device *dev, uint32_t fport, uint32_t lport) {
    return dgmopen(dev, (void*)fport, (void*)lport);
}

int
udp_m_iocntl(struct device *dev, int op, void* arg1, void* arg2) {
	return dgcntl(dev, op, arg1);
}


