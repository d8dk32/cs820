#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"

void fakeFinalizer(void* arg){
    return;
}

int main(int argc, char** argv){

    if (argc != 2){
        printf("no test number specified\n");
        return -1;
    }

    int testNum = atoi(argv[1]);

    if (testNum == 1){
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, fakeFinalizer);

        *(allocated + 12UL) = (unsigned long) allocated+12UL;

        unsigned long* a2 = memAllocate(10, NULL);
        *(a2 + 5) = (unsigned long) 0x1122334455667788;
        memAllocate(42, NULL);

        memDump();
    }
}