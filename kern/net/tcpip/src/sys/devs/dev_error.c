#include <tcpip/h/network.h>


int 
error_init(struct device *dev) {
    panic("error_init not be implemented \n");
	return OK;
}

int
error_open(struct device *dev, uint32_t open_flags, uint32_t arg2) {
    panic("error_open not be implemented \n");
    return(dev->dvnum);
}

int
error_close(struct device *dev) {
   panic("error_close not be implemented \n");
   return OK;
}

int
error_io(struct device *dev, struct iobuf *iob, bool write) {
   panic("error_io not be implemented \n");
   return OK;
}

int
error_ioctl(struct device *dev, int op, void* arg1, void* arg2) {
	panic("error_ioctl not be implemented \n");
   return OK;
}


int 
error_write(struct device *dev, char* buff, int len) {
	panic("error_write not be implemented \n");
   return OK;
}

int 
error_read(struct device *dev, char* buff, int len) {
	panic("error_read not be implemented \n");
   return OK;
}


