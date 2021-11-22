#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"

static int isInitialized = 0;

static unsigned long* heapBasePtr;

static int inFinalizeCall = 0;

int memInitialize(unsigned long size){
    if (isInitialized == 1){
        return 0; //can't re-initialize
    } else {
        isInitialized = 1;
    }

    if (size <= 0){
        return 0; // size must be > 0
    }

    heapBasePtr = malloc((8*size)+16);
    if (heapBasePtr == NULL){
        return 0; //malloc failure
    }
    
    *heapBasePtr = size;

    return 1;
}


long* findSpace( unsigned long size, long* startingPoint){
    if (*heapBasePtr >> 64 == 0 && *heapBasePtr & 0xFFFFFFFFFFFFFFFF >= size+2){
        //this is enough space
    } else {
        //recursively call findSpace on the next chunk.
        //what's the base case for recursion? how do I know when there isn't a 'next chunk'?
    }
    }
}

void *memAllocate(unsigned long size, void (*finalize)(void *)){
    if (inFinalizeCall == 1){
        fprintf(stderr, "Cannot call memAllocate from destructor.\n");
        exit(-1);
    }

}

void callDestructor(void (*finalize)(void *), long* blockBaseAddr){
    inFinalizeCall = 1;
    finalize(blockBaseAddr);
    inFinalizeCall = 0;
}

static void collectGarbage(){
    return;
}
void memDump(void);