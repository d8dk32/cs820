#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"

static unsigned long initializedSpace = 0;

static unsigned long* heapBasePtr;

static int inFinalizeCall = 0;


//[-][-][-][------------------------------------------------------------]
// highest bit - allocated
// 2nd highest - mark for collection
// 3rd highest - reachable, used to stop recursion during mark phase (don't forget to reset after sweep phase)
//remaining bits - size of chunk

int memInitialize(unsigned long size){
    if (initializedSpace != 0){
        return 0; //can't re-initialize
    } else if (size <= 0){
        return 0; // size must be > 0
    } else {
        initializedSpace = size;
    }

    heapBasePtr = malloc((8*size)+16);
    if (heapBasePtr == NULL){
        return 0; //malloc failure
    }
    
    *heapBasePtr = 0x0000000000000000 | size;

    return 1;
}


static unsigned long* findSpace( unsigned long size, long* startingPoint){

    unsigned long curChunkSize = (*startingPoint & 0x1FFFFFFFFFFFFFFF);
    unsigned long curAllocBit = (*startingPoint & 0x8FFFFFFFFFFFFFFF) >> 63;

    if (curAllocBit == 0 && curChunkSize > size+2) {
        //this is enough space
        *startingPoint = 0x8000000000000000 | size; //set alloc bit and size

        if (size+1 < curChunkSize) {    
            //allocated chunk doesn't use all the space  
            // so set up next chunk header
            unsigned long* nextChunkStart = startingPoint+size+2;
            *nextChunkStart = curChunkSize-size-2;
        }

        return startingPoint;     
        
    } else {
        //recursively call findSpace on the next chunk.
       
        //check recursive base case - we've reached the end of the heap

        unsigned long* nextChunkStart = startingPoint+curChunkSize+2;

        if (nextChunkStart > heapBasePtr+initializedSpace){
            return NULL;
        }

        return findSpace(size, startingPoint+curChunkSize+2);
    
    }
}

static void callDestructor(void (*finalize)(void *), long* blockBaseAddr){
    inFinalizeCall = 1;
    finalize(blockBaseAddr);
    inFinalizeCall = 0;
}

static void collectGarbage(){
    return;
}

void *memAllocate(unsigned long size, void (*finalize)(void *)){
    if (inFinalizeCall == 1){
        fprintf(stderr, "Cannot call memAllocate from destructor.\n");
        exit(-1);
    }

    if (initializedSpace == 0){
        return NULL; //heap has not been initialized
    }

    unsigned long* blockBaseAddr = findSpace(size, heapBasePtr);
    if (blockBaseAddr != NULL) {
        *(blockBaseAddr+1) = (unsigned long) finalize;
        return (void*) blockBaseAddr+2;
    }

    collectGarbage();

    return (void*) findSpace(size, heapBasePtr);

}

static void dumpHeap(void){

    printf("Heap - 2 word header:\n");

    unsigned long* startingPoint = heapBasePtr;
    while(startingPoint < heapBasePtr + initializedSpace) {
        unsigned long curChunkSize = (*startingPoint & 0x1FFFFFFFFFFFFFFF);
        unsigned long curAllocBit = (*startingPoint & 0x8FFFFFFFFFFFFFFF) >> 63;
        unsigned long curMarkBit = (*startingPoint & 0x8FFFFFFFFFFFFFFF) >> 62;
        
        printf("Block - %ld allocated - %s - %s - %016lx\n", curChunkSize, 
            curAllocBit==1 ? "Allocated":"Free", curMarkBit==1 ? "Marked":"Unmarked", (long) *(startingPoint+1) );
        
        if(curAllocBit == 1){
            //print contents of this chunk
        }

        startingPoint += curChunkSize+2;

    }
}

void memDump(void){
    //just dumping the heap to start so I can see if memalloc works

    dumpHeap();


}
