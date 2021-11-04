#include <stdio.h>
#include "thread.h"


extern void asm_yield(TCB* curTCB, TCB* nextTCB);
extern void thread_start(void (*work)(void*), void* arg);

TCB* readyQueueHead = NULL;

TCB* readyQueueTail(){
    if (readyQueueHead == NULL) return NULL;
    TCB* curTCB = readyQueueHead;
    while(curTCB->next != NULL) {
        curTCB = curTCB->next;
    }
    return curTCB;
}

void cleanup(){
    //what does this do?
}

//the secret primitives
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

    readyQueueTail()->next = newTCB;    
    //set up thread's stack

}

void thread_yield(void){

}

int main(int argc, char** argv){

}