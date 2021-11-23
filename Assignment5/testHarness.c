#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"

int main(int argc, char** argv){

    if (argc != 2){
        printf("no test number specified\n");
        return -1;
    }

    int testNum = atoi(argv[1]);

    if (testNum == 1){
        int init = memInitialize(100);
        long* allocated = (long*) memAllocate(20, NULL);

        memAllocate(10, NULL);
        memAllocate(42, NULL);

        memDump();
    }
}