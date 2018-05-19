#include <tcpip/h/network.h>

int 
null_init(struct device *dev) {
	return OK;
}

int
null_open(struct device *dev, uint32_t open_flags, uint32_t arg2) {
    return(dev->dvnum);
}

int
null_close(struct device *dev) {
   return OK;
}

int
null_io(struct device *dev, struct iobuf *iob, bool write) {
   return OK;
}

int
null_ioctl(struct device *dev, int op, void* arg1, void* arg2) {
   return OK;
}


int null_write(struct device *dev, char* buff, int len) {
   return OK;
}

int null_read(struct device *dev, char* buff, int len) {
   return OK;
}

