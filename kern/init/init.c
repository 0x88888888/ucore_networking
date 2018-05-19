#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <kdebug.h>
#include <kmonitor.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>
#include <vmm.h>
#include <ide.h>
#include <swap.h>
#include <proc.h>
#include <fs.h>
#include <sync.h>
#include <sockets.h>
#include <net.h>
#include <netcheck.h>

void kern_init(void) __attribute__((noreturn));
void grade_backtrace(void);

void meminfo(char* pre) {
    extern free_area_t free_area;

   #define free_list (free_area.free_list)
   #define nr_free (free_area.nr_free)


    int count = 0, total = 0;
    list_entry_t *le = &free_list;
    while ((le = list_next(le)) != &free_list) {
        struct Page *p = le2page(le, page_link);
        assert(PageProperty(p));
        count ++, total += p->property;
    }
    assert(total == nr_free_pages());
    cprintf("count= %d total memory= %x bytes \n",count,total);
    panic(pre);

}

void
kern_init(void){
	extern char edata[], end[];
    // 设置.bss section内存值 0
    memset(edata, 0, end - edata);
    
    cons_init();                // init the console

    const char *message ="(Mike.Thx) os is loading...";
    cprintf("%s\n\n",message);

    print_kerninfo();

    grade_backtrace();

    pmm_init();                 // init physical memory management
    //meminfo("pmm_init finished\n");
    pic_init();                 // init interrupt controller
    idt_init();                 // init interrupt descriptor
    
    vmm_init();                 // init virtual memory management
    sched_init();               // init scheduler
    sync_init();                // init sync struct

    proc_init();                // init process table
    
    ide_init();                 // init ide devices
    swap_init();                // init swap
    fs_init();                  // init fs
    
    //100次时钟中断/秒
    clock_init();               // init clock interrupt
    
    
    net_init();                  // init nic
    net_check();
    
    intr_enable();              // enable irq interrupt
    cpu_idle();                 // run idle process
}

void __attribute__((noinline))
grade_backtrace2(int arg0, int arg1, int arg2, int arg3) {
    mon_backtrace(0, NULL, NULL);
}

void __attribute__((noinline))
grade_backtrace1(int arg0, int arg1) {
    grade_backtrace2(arg0, (int)&arg0, arg1, (int)&arg1);
}

void __attribute__((noinline))
grade_backtrace0(int arg0, int arg1, int arg2) {
    grade_backtrace1(arg0, arg2);
}

void
grade_backtrace(void) {
    grade_backtrace0(0, (int)kern_init, 0xffff0000);
}
