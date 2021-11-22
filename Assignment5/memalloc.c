#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"

static int isInitialized = 0;

static long* heapBasePtr;

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
    
    return 1;
}

void *memAllocate(unsigned long size, void (*finalize)(void *));
void memDump(void);