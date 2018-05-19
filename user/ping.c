
#include <ulib.h>
#include <stdio.h>
#include <string.h>

int 
main(int argc, char const *argv[]) {
    int len=0;
    if (argc != 2) {
    	cprintf("usage : ping host\n");
    }
    len = strlen(argv[1]);
    ping(argv[1], len);

    return 0;
}


