#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"

void fakeFinalizer(void* arg){
    printf("Fake finalizer\n");
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

        unsigned long* a2 = memAllocate(10, NULL);
        *(a2 + 5) = (unsigned long) 0x1122334455667788;
        unsigned long* a3 = memAllocate(42, fakeFinalizer);

        *(allocated + 7) = (unsigned long) (allocated);
        *(allocated + 8) = (unsigned long) (allocated + 7);

        *(a3 + 20) = (unsigned long) (a2+3); 

        a3 = NULL;


        memAllocate(100, NULL);

        memDump();
    }
}