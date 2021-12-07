#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"

void fakeFinalizer(void* arg){
    printf("Fake finalizer\n");
}


void funcThatAllocates1(){
    unsigned long* allocated = memAllocate(20, NULL);
    memDump();
}
void funcThatAllocates2(){
    unsigned long* allocated = memAllocate(20, NULL);
    funcThatAllocates1();
}
void funcThatAllocates3(){
    unsigned long* allocated = memAllocate(20, NULL);
    funcThatAllocates2();
}

void funcThatAllocatesC(){
    unsigned long* allocated = memAllocate(40, NULL);
    memAllocate(60, NULL);
    memDump();
}
void funcThatAllocatesB(){
    unsigned long* allocated = memAllocate(20, NULL);
    funcThatAllocatesC();
}
void funcThatAllocatesA(){
    unsigned long* allocated = memAllocate(20, NULL);
    funcThatAllocatesB();
}

int main(int argc, char** argv){

    if (argc != 2){
        printf("no test number specified\n");
        return -1;
    }

    int testNum = atoi(argv[1]);

    if (testNum == 1){
        //simple allocate test
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, fakeFinalizer);
        memDump();
    }
    if (testNum == 2){
        //attempting to allocate more than was initialized
        int init = memInitialize(10);
        unsigned long* allocated = memAllocate(20, NULL);
        memDump();
    }
    if (testNum == 3){
        //simple allocation, then attempting to allocate more space than is left
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        unsigned long* allocated2 = memAllocate(90, NULL);
        memDump();
        printf("Return value of 2nd allocation attempt: 0x%ld\n", (unsigned long) allocated2);
    }
    if (testNum == 4){
        //make sure memdump correctly shows pointers on the stack
        int init = memInitialize(100);
        funcThatAllocates3();
    }
    if (testNum == 5){
        //make sure memdump correctly flags pointers in the heap
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated+2) = (unsigned long) memAllocate(20, NULL);
        memDump();
    }
    if (testNum == 6){
        //confirm that block gets collected if reference is not saved
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        memAllocate(20, NULL);
        memDump();
        memAllocate(60, NULL);
        memDump();
    }
    if (testNum == 7){
        //ensure reference saved on heap prevents collection
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated+2) = (unsigned long) memAllocate(20, NULL);
        memDump();
        memAllocate(60, NULL);
        memDump();
    }
    if (testNum == 8){
        //test block coalescing during collection, confirm finalizer is called
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        memAllocate(20, fakeFinalizer);
        memAllocate(20, fakeFinalizer);
        unsigned long* allocated2 = memAllocate(20, NULL);
        memDump();
        memAllocate(70,NULL);
        memDump();
        memAllocate(30,NULL);
        memDump();
    }
    if (testNum == 9){
        //test that pointers on that stack prevent coalescing
        int init = memInitialize(100);
        funcThatAllocatesA();
    }
    if (testNum == 10){
        //test chained pointers in heap - should prevent collection
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated + 2) = (unsigned long) memAllocate(20, NULL);
        *((unsigned long *) *(allocated + 2)) = (unsigned long) memAllocate(20, NULL);
        memDump();
        memAllocate(50, NULL);
        memDump();
    }
    if (testNum == 11){
        //test looping pointers in heap - shouldn't blow up
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated + 2) = (unsigned long) memAllocate(20, NULL);
        *((unsigned long *) *(allocated + 2)) = (unsigned long) allocated;
        memDump();
        memAllocate(70, NULL);
        memDump();

    }

}