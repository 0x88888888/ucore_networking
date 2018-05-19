#include <stdio.h>
#include <ulib.h>
#include <malloc.h>

int
main(void) {
    cprintf("Hello world!!.\n");
    cprintf("I am process %d.\n", getpid());
    cprintf("hello pass.\n");
    
 
 
    char buffer[1024]= {0};
    cprintf("buffer addr = %x \n",buffer);

    return 0;
}

