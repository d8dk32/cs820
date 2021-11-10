#include <stdlib.h>
#include <stdio.h>
#include "thread.h"


extern void asm_yield(TCB* curTCB, TCB* nextTCB);
extern void asm_yield_cleanup(TCB* curTCB, TCB* nextTCB);
//extern void thread_start(void (*work)(void*), void* arg);

TCB* readyQueueHead = NULL;

TCB* waitingForCleanup = NULL;

TCB* getReadyQueueTail(){
    if (readyQueueHead == NULL) return NULL;
    TCB* curTCB = readyQueueHead;
    while(curTCB->next != NULL) {
        curTCB = curTCB->next;
    }
    return curTCB;
}

TCB* getReadyQueueHead(){
    return readyQueueHead;
}

void setReadyQueueHead(TCB* newHead){
    readyQueueHead = newHead;
}

int readyQueueLength(){
    if (readyQueueHead == NULL) return 0;
    int len = 1;
    TCB* curTCB = readyQueueHead;
    while(curTCB->next != NULL) {
        len++;
        curTCB = curTCB->next;
    }
    return len;
}

//the secret primitives - cleanup and threadStart
void cleanup(){
    //chop the to-be-cleaned-up TCB out of the ready queue
    printf("made it to cleanup\n");
    TCB* cleanupTCB = readyQueueHead;
    readyQueueHead = readyQueueHead->next;
    cleanupTCB->next = NULL;

    if(waitingForCleanup != NULL){
        free(waitingForCleanup->stackPtr);
        free(waitingForCleanup);
    } 
    
    waitingForCleanup = cleanupTCB;
    
    asm_yield_cleanup(NULL, readyQueueHead);
}


void threadStart(void (*work)(void*) , void* arg){ 
    printf("starting thread...\n");
    work(arg);
    cleanup();
}

long thread_create(void (*work)(void*) , void* arg){

    if (readyQueueHead == NULL){
        //make TCB for main thread
        readyQueueHead = malloc(sizeof(TCB)); // does the main thread even need its own stack? don't think so
        readyQueueHead->next = NULL;
        readyQueueHead->stackPtr = malloc(65536);
    }
    printf("threadStart addr: %lx", (long) threadStart);
    TCB* newTCB = malloc(sizeof(TCB));
    newTCB->next = NULL;
    newTCB->stackPtr = (long*) malloc(65536);
    *(newTCB->stackPtr) = 0;
    *(newTCB->stackPtr+1) = (long) threadStart; //<-THIS IS THE ADDRESS THAT GETS RETURNED TO AFTER ASM YIELD COMPLETES
    *(newTCB->stackPtr+2) = 0;
    newTCB->rsp = (long) newTCB->stackPtr;
    newTCB->rdi = (long) work;
    newTCB->rsi = (long) arg;
    //dumping 0 into the rest of the slots - is this needed?
    newTCB->rbx = 0;
    newTCB->r12 = 0;
    newTCB->r13 = 0;
    newTCB->r14 = 0;
    newTCB->r15 = 0;
    newTCB->heldMutex = NULL;

    getReadyQueueTail()->next = newTCB;    
    //set up thread's stack

    return (long) newTCB;
}

void thread_yield(void){
    if (readyQueueHead == NULL){
        //make TCB for main thread
        printf("making new TCB for main thread\n");
        readyQueueHead = malloc(sizeof(TCB));
        readyQueueHead->next = NULL;
        readyQueueHead->stackPtr = malloc(65536);
    }
    if (readyQueueHead->next == NULL) {
        //only 1 thread active, continue execution of current thread
        printf("Only 1 thread exists, continuing execution...\n");
        return;
    }

    TCB* curTCB = readyQueueHead;
    readyQueueHead = curTCB->next;
    getReadyQueueTail()->next = curTCB;
    curTCB->next = NULL;
    printf("calling asm_yield with curTid %ld and nextTid %ld\n", (long) curTCB, (long) readyQueueHead);
    printf("ready list head: %ld, ready list tail: %ld\n", (long) getReadyQueueHead(), (long)getReadyQueueTail());
    asm_yield(curTCB, readyQueueHead);
}

long thread_self(void){
    return (long) readyQueueHead;
}