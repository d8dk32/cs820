#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"

void fakeFinalizer(void* arg){
    printf("Finalizer Called\n");
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
        printf("Test 1: Simple allocate test\n - Initialize 100 Words \n - Allocate 20 words\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, fakeFinalizer);
        memDump();
    }
    if (testNum == 2){
        printf("Test 2: attempting to allocate more than was initialized\n");
        printf(" - Initialize 10 words \n - Attempt to allocate 20 words\n");
        int init = memInitialize(10);
        unsigned long* allocated = memAllocate(20, NULL);
        memDump();
        printf("Return value of 2nd allocation attempt: 0x%ld\n", (unsigned long) allocated);
    }
    if (testNum == 3){
        printf("Test 3: Simple allocation, then attempting to allocate more space than is left\n");
        printf(" - initialize 100 words \n - allocate 20 words \n - attempt to allocate 90 words\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        unsigned long* allocated2 = memAllocate(90, NULL);
        memDump();
        printf("Return value of 2nd allocation attempt: 0x%ld\n", (unsigned long) allocated2);
    }
    if (testNum == 4){
        printf("Test 4: make sure memdump correctly shows pointers on the stack\n");
        printf(" - Initialize 100 words\n - 3 nested function calls each allocate 20 words, last one calls memDump\n");
        printf(" - stack output should indicate at least 3 reachable blocks\n");
        int init = memInitialize(100);
        funcThatAllocates3();
    }
    if (testNum == 5){
        printf("Test 5: make sure memdump correctly flags pointers in the heap\n");
        printf(" - initialize 100 words \n - allocate 20 words \n - allocate 20 more, store result in first block\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated+2) = (unsigned long) memAllocate(20, NULL);
        memDump();
    }
    if (testNum == 6){
        printf("Test 6: confirm that block gets collected if reference is not saved\n");
        printf(" - intialize 100 words\n - allocate 20 words, storing returned pointer in a local variable\n");
        printf(" - allocate 20 more words but do not save return value\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        memAllocate(20, NULL);
        memDump();
        printf(" - now allocate 60 words. GC should free the unreachable block of 20 words to make space\n");
        memAllocate(60, NULL);
        memDump();
    }
    if (testNum == 7){
        printf("Test 7: ensure reference saved on heap prevents collection\n");
        printf(" - initialize 100 words\n - allocate 20 words\n - allocate 20 more words, storing the returned pointer inside the first allocated block\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated+2) = (unsigned long) memAllocate(20, NULL);
        memDump();
        printf(" - attempt to allocate 60 words should fail. GC should not collect either block of 20\n");
        memAllocate(60, NULL);
        memDump();
    }
    if (testNum == 8){
        printf("Test 8: test block coalescing during collection, confirm finalizer is called\n");
        printf(" - initialize 100 words\n - allocate block of 20, saving returned pointer in a local variable\n");
        printf(" - allocate two blocks of 20 words, do not save their returned pointers\n");
        printf(" - allocate block of 20, saving returned pointer in a local variable\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        memAllocate(20, fakeFinalizer);
        memAllocate(20, fakeFinalizer);
        unsigned long* allocated2 = memAllocate(20, NULL);
        memDump();
        printf(" - attempt to allocate 70 words. This should fail even after GC, but the middle 2 blocks of 20 should coalesce and finalizer should be called\n");
        memAllocate(70,NULL);
        memDump();
        printf(" - attempt to allocate 30 words. This should succeed, and the block should fill part of the freed and coalesced space\n");
        memAllocate(30,NULL);
        memDump();
    }
    if (testNum == 9){
        printf("Test 9: test that pointers on that stack prevent collection\n");
        printf(" - initialize 100 words\n - nested function calls allocate 2 blocks of 20 and a block of 40, saving the returned pointers in function-local variables\n");
        printf(" - attempt to allocate 60 words, which should fail. Previously allocated blocks should not be collected\n");
        int init = memInitialize(100);
        funcThatAllocatesA();
    }
    if (testNum == 10){
        printf("Test 10: test chained pointers in heap - should prevent collection\n");
        printf(" - initialize 100 words\n - allocate 20 words, storing returned pointer in local variable\n");
        printf(" - allocate 20 words, storing returned pointer inside first allocatd block\n");
        printf(" - allocate 20 words, storing returned pointer inside second allocated block\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated + 2) = (unsigned long) memAllocate(20, NULL);
        *((unsigned long *) *(allocated + 2)) = (unsigned long) memAllocate(20, NULL);
        memDump();
        printf(" - attempt to allocate 50 words, which should fail. GC should not collect previously allocated blocks\n");
        memAllocate(50, NULL);
        memDump();
    }
    if (testNum == 11){
        printf("Test 11: test looping pointers in heap - shouldn't cause an error\n");
        printf(" - initialize 100 words\n - allcoate block of 20 words\n - allocate another block of 20, storing returned value inside first allocated block\n");
        printf(" - store a pointer to first allocated block inside 2nd block\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        *(allocated + 2) = (unsigned long) memAllocate(20, NULL);
        *((unsigned long *) *(allocated + 2)) = (unsigned long) allocated;
        memDump();
        printf(" - attempt to allocate 70 words. This should fail. GC should not collect previously allocated blocks\n");
        memAllocate(70, NULL);
        memDump();

    }
    if (testNum == 12){
        printf("Test 12: looping pointers in heap, except that they point to the middle of blocks instead of the start\n");
        printf(" - initialize 100 words\n - allocate 2 blocks of 20, saving returned pointers in local variables\n");
        printf(" - inside each block store a pointer to the middle of the other\n");
        printf(" - Zero out the reference to one of the allocated blocks\n");
        int init = memInitialize(100);
        unsigned long* allocated = memAllocate(20, NULL);
        unsigned long* allocated2 = memAllocate(20, NULL);
        *(allocated + 2) = (unsigned long) (allocated2 + 4);
        *(allocated2 + 2) = (unsigned long) (allocated +1);
        allocated = NULL; 
        memDump();
        printf(" - attempt to allocate 70 words. This should fail. GC should not collect either previously allocated block\n");
        memAllocate(70, NULL);
        memDump();

    }

}