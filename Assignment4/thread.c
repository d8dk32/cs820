#include <stdlib.h>
#include <stdio.h>
#include "thread.h"


extern void asm_yield(TCB* curTCB, TCB* nextTCB);
extern void asm_yield_cleanup(TCB* curTCB, TCB* nextTCB);

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
    //printf("made it to cleanup\n");
    TCB* cleanupTCB = readyQueueHead;
    readyQueueHead = readyQueueHead->next;
    cleanupTCB->next = NULL;

    if(waitingForCleanup != NULL){
        free(waitingForCleanup->stackBottom);
        free(waitingForCleanup);
    } 
    
    waitingForCleanup = cleanupTCB;
    
    asm_yield_cleanup(NULL, readyQueueHead);
}


void threadStart(void (*work)(void*) , void* arg){ 
    //printf("starting thread...\n");
    work(arg);
    cleanup();
}

long thread_create(void (*work)(void*) , void* arg){

    if (readyQueueHead == NULL){
        //make TCB for main thread
        readyQueueHead = malloc(sizeof(TCB)); 
        readyQueueHead->next = NULL;
        readyQueueHead->stackPtr = 0; //malloc(65536); // does the main thread even need its own stack? don't think so
        readyQueueHead->rsp = 0;
        readyQueueHead->rdi = 0;
        readyQueueHead->rsi = 0;
        readyQueueHead->rbx = 0;
        readyQueueHead->r12 = 0;
        readyQueueHead->r13 = 0;
        readyQueueHead->r14 = 0;
        readyQueueHead->r15 = 0;
        readyQueueHead->heldMutex = NULL;
    }
    //printf("threadStart addr: %lx\n", (long) threadStart);
    long* temp = malloc(65536);
    TCB* newTCB = malloc(sizeof(TCB));
    newTCB->stackBottom = temp;
    newTCB->next = NULL;
    newTCB->stackPtr = temp+8189;
    *(newTCB->stackPtr) = 0;
    *(newTCB->stackPtr+1) = (long) threadStart; //<-THIS IS THE ADDRESS THAT GETS RETURNED TO AFTER ASM YIELD COMPLETES
    *(newTCB->stackPtr+2) = 0;
    newTCB->rsp = (long) newTCB->stackPtr+0;

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

    return (long) newTCB;
}

void thread_yield(void){
    if (readyQueueHead == NULL){
                //make TCB for main thread
        readyQueueHead = malloc(sizeof(TCB)); 
        readyQueueHead->next = NULL;
        readyQueueHead->stackPtr = 0; //malloc(65536); // does the main thread even need its own stack? don't think so
        readyQueueHead->rsp = 0;
        readyQueueHead->rdi = 0;
        readyQueueHead->rsi = 0;
        readyQueueHead->rbx = 0;
        readyQueueHead->r12 = 0;
        readyQueueHead->r13 = 0;
        readyQueueHead->r14 = 0;
        readyQueueHead->r15 = 0;
        readyQueueHead->heldMutex = NULL;
    }
    if (readyQueueHead->next == NULL) {
        //only 1 thread active, continue execution of current thread
        //printf("Only 1 thread exists, continuing execution...\n");
        return;
    }

    TCB* curTCB = readyQueueHead;
    readyQueueHead = readyQueueHead->next;
    curTCB->next = NULL;
    getReadyQueueTail()->next = curTCB;  
    //printf("calling asm_yield with curTid %ld and nextTid %ld\n", (long) curTCB, (long) readyQueueHead);
    //printf("ready list head: %ld, ready list tail: %ld\n", (long) getReadyQueueHead(), (long)getReadyQueueTail());
    //printf("old head next: %ld\n", (long) curTCB->next);
    asm_yield(curTCB, readyQueueHead);
}

long thread_self(void){
    return (long) readyQueueHead;
}