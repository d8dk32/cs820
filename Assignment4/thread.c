#include <stdlib.h>
#include <stdio.h>
#include "thread.h"


extern void asm_yield(TCB* curTCB, TCB* nextTCB);
extern void thread_start(void (*work)(void*), void* arg);

TCB* readyQueueHead = NULL;

TCB* cleanupList = NULL;

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
    //what does this do?
}


void threadStart(void (*work)(void*) , void* arg){ 
    (*work)(arg);
    cleanup();
}

long thread_create(void (*work)(void*) , void* arg){

    if (readyQueueHead == NULL){
        //make TCB for main thread
        readyQueueHead = malloc(sizeof(TCB));
        readyQueueHead->next = NULL;
        readyQueueHead->stackPtr = malloc(65536);
    }

    TCB* newTCB = malloc(sizeof(TCB));
    newTCB->next = NULL;
    newTCB->stackPtr = malloc(65536);
    *newTCB->stackPtr = 0;
    *(newTCB->stackPtr+1) = (long) threadStart; //<-THIS IS THE ADDRESS THAT GETS RETURNED TO AFTER ASM YIELD COMPLETES(?)
    *(newTCB->stackPtr+2) = 0;
    newTCB->rsp = (long) newTCB->stackPtr+2;
    newTCB->rdi = (long) work;
    newTCB->rsi = (long) arg;

    getReadyQueueTail()->next = newTCB;    
    //set up thread's stack

    return (long) newTCB;
}

void thread_yield(void){
    if (readyQueueHead == NULL){
        //make TCB for main thread
        readyQueueHead = malloc(sizeof(TCB));
        readyQueueHead->next = NULL;
        readyQueueHead->stackPtr = malloc(65536);
    }
    if (readyQueueHead->next == NULL) {
        //only 1 thread active, continue execution of current thread
        return;
    }

    TCB* curTCB = readyQueueHead;
    readyQueueHead = curTCB->next;
    getReadyQueueTail()->next = curTCB;
    curTCB->next = NULL;
    //asm_yield(curTCB, readyQueueHead);
}

long thread_self(void){
    return (long) readyQueueHead;
}