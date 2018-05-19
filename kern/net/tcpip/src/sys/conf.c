#include <tcpip/h/network.h>

struct device devtab[NDEVS] = {
    {
     .dvnum = 0,
     .name="undefined",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = null_init,
     .d_open = null_open,
     .d_write= null_write,
     .d_read = null_read,
     .d_close = null_close,
     .d_io = null_io,
     .d_ioctl = null_ioctl,
     .dvioblk = "undefined",
     .dvminor= 0
   },
   // ec0
   {
     .dvnum = 1,
     .name="EC0",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = dev_e1000_init,
     .d_open = dev_e1000_open,
     .d_write = dev_e1000_write,
     .d_read = dev_e1000_read,
     .d_close = dev_e1000_close,
     .d_io = dev_e1000_io,
     .d_ioctl = dev_e1000_ioctl,
     .dvioblk = "EC0",
     .dvminor= 0 ,// eth[]索引值
   },
   //udp主设备
   {
     .dvnum = 2, //devtab 索引
     .name="dgm", //manager 
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = null_init,
     .d_open = udp_m_open,
     .d_close = null_close,
     .d_write= error_write,
     .d_read = error_read,
     .d_io =    error_io,
     .d_ioctl = udp_m_iocntl,//调用 dgmcntl
     .dvioblk = NULLPTR,
     .dvminor= 0, //未用
   },
   {
     .dvnum = 3, //devtab 索引
     .name="udp0",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR, //dgblk对象
     .dvminor= 0,  //dgtab索引
   },
   {
     .dvnum = 4,
     .name="udp1",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR,//dgblk对象
     .dvminor= 1, //dgtab索引
   },
   {
     .dvnum = 5,
     .name="udp2",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR,//dgblk对象
     .dvminor= 2, //dgtab索引
   },
   {
     .dvnum = 6,
     .name="udp3",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR,//dgblk对象
     .dvminor= 3, //dgtab索引
   },
   {
     .dvnum = 7,
     .name="udp4",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR,//dgblk对象
     .dvminor= 4, //dgtab索引
   },
   {
     .dvnum = 8,
     .name="udp5",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR,//dgblk对象
     .dvminor= 5, //dgtab索引
   },
   {
     .dvnum = 9,
     .name="udp6",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR,//dgblk对象
     .dvminor= 6, //dgtab索引
   },
   {
     .dvnum = 10,
     .name="udp7",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = udp_init,
     .d_open = null_open,
     .d_close = udp_close,
     .d_write= udp_write,
     .d_read = udp_read,
     .d_io = error_io,
     .d_ioctl = udp_ioctl,
     .dvioblk = NULLPTR,//dgblk对象
     .dvminor= 7, //dgtab索引
   },
   //tcp 主设备
   //tcp
   {
     .dvnum = 11,
     .name="tcp_m",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = null_init,
     .d_open = tcp_m_open,
     .d_close = null_close,
     .d_write = error_write,
     .d_read  = error_read,
     .d_io    = error_io,
     .d_ioctl = tcp_m_iocntl,
     .dvioblk = NULLPTR,
     .dvminor= 0,  //未用
   },
   {
     .dvnum = 12,
     .name="tcp0",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = error_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR, //tcb对象
     .dvminor= 0, //tcbtab索引
   },
   {
     .dvnum = 13,
     .name="tcp1",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = tcp_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR, //tcb对象
     .dvminor= 1,//tcbtab索引
   },
   {
     .dvnum = 14,
     .name="tcp2",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = tcp_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR,//tcb对象
     .dvminor= 2, //tcbtab索引
   },
   {
     .dvnum = 15,
     .name="tcp3",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = tcp_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR,//tcb对象
     .dvminor= 3, //tcbtab索引
   },
   {
     .dvnum = 16,
     .name="tcp4",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = tcp_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR,//tcb对象
     .dvminor= 4, //tcbtab索引
   },
   {
     .dvnum = 17,
     .name="tcp5",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = tcp_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR,//tcb对象
     .dvminor= 5,  //tcbtab索引
   },
   {
     .dvnum = 18,
     .name="tcp6",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = tcp_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR,//tcb对象
     .dvminor= 6,  //tcbtab索引
   },
   {
     .dvnum = 19,
     .name="tcp7",
     .d_blocks = 0,
     .d_blocksize = 0,
     .d_init = tcp_init,
     .d_open = null_open,
     .d_close = tcp_close,
     .d_io = tcp_io,
     .d_write = tcp_write,
     .d_read  = tcp_read,
     .d_ioctl = tcp_ioctl,
     .dvioblk = NULLPTR,//tcb对象
     .dvminor= 7,  //tcbtab索引
   },
   
}; 
