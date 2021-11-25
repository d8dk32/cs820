#include <stdlib.h>
#include <stdio.h>
#include "memalloc.h"

extern unsigned long __data_start;
extern unsigned long _end;

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

//returns 0 if the input addr doesn't point to an allocated block, and returns a pointer to the header
// of the block that contains the input addr, if such a block exists
unsigned long* checkIfValueIsAllocatedBlockAddr(unsigned long value){
    if (value < (unsigned long) heapBasePtr || value > (unsigned long) heapBasePtr + initializedSpace + 2){
        //this address is outside the range of the heap so we can bail right now
        return 0;
    }
    //next, check if the block it would point to is allocated or free
    unsigned long* startingPoint = heapBasePtr;
    while((unsigned long) startingPoint < (unsigned long) (heapBasePtr + initializedSpace)) {
        unsigned long curChunkSize = (*startingPoint & 0x1FFFFFFFFFFFFFFF); 
        unsigned long curAllocBit = (*startingPoint & 0x8FFFFFFFFFFFFFFF) >> 63;

        if (value >= (unsigned long) startingPoint && value <= (unsigned long) (startingPoint+curChunkSize+2) && curAllocBit == 1) {
            return startingPoint;
        }
    
        startingPoint += curChunkSize+2;
    }

    return 0;

}


static unsigned long* findSpace( unsigned long size, unsigned long* startingPoint){

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
        return blockBaseAddr+2;
    }

    collectGarbage();

    blockBaseAddr = findSpace(size, heapBasePtr);
    if (blockBaseAddr != NULL) {
        *(blockBaseAddr+1) = (unsigned long) finalize;
        return blockBaseAddr+2;
    }

    return NULL;

}

static void dumpGlobalMem(void){
    unsigned long* globalMemStart = &__data_start;
    unsigned long* globalMemEnd = &_end;

    printf("Global Memory: start=0x%016lx, end=0x%016lx, length=%ld\n", 
        (unsigned long) globalMemStart, 
        (unsigned long) globalMemEnd, 
        (unsigned long) (globalMemEnd-globalMemStart));
    
    for (unsigned long i = 0; i < (unsigned long) (globalMemEnd-globalMemStart); i++){
        printf(" 0x%016lx %016lx%s\n", 
            (unsigned long) (globalMemStart+i), 
            (unsigned long) *(globalMemStart+i), 
            checkIfValueIsAllocatedBlockAddr((unsigned long) *(globalMemStart+i)) != NULL ? "*":"");
    }
    printf("\n");
}

static void dumpHeap(void){

    printf("Heap - 2 word header:\n");

    unsigned long* startingPoint = heapBasePtr;
    while(startingPoint < heapBasePtr + initializedSpace) {
        unsigned long curChunkSize = (*startingPoint & 0x1FFFFFFFFFFFFFFF);
        unsigned long curAllocBit = (*startingPoint & 0x8FFFFFFFFFFFFFFF) >> 63;
        unsigned long curMarkBit = (*startingPoint & 0x8FFFFFFFFFFFFFFF) >> 62;
        
        printf("\nBlock - %ld words - %s - %s ", curChunkSize, 
            curAllocBit==1 ? "Allocated":"Free", curMarkBit==1 ? "Marked":"Unmarked");
        if (curAllocBit == 1){
            printf("- %016lx\n", (unsigned long) *(startingPoint+1));
        } else {
            printf("\n");
        }
        
        if(curAllocBit == 1){
            //print contents of this chunk
            for(unsigned long i = 0; i < curChunkSize; i++){
                if(i%7 == 0){
                    printf("\n 0x%016lx: ", (unsigned long) (startingPoint+2+i));
                }
                printf("%016lx", *(startingPoint+2+i));

                if(checkIfValueIsAllocatedBlockAddr((unsigned long) *(startingPoint+2+i)) != NULL){
                    printf("* ");
                } else {
                    printf("  ");
                }
            }
            printf("\n");
        }

        startingPoint += curChunkSize+2UL;

    }
}

void memDump(void){

    dumpGlobalMem();


    dumpHeap();


}
