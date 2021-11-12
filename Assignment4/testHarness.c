#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

// some utility functions for testing---------------------------------------------------------------
void deleteAllThreads(){
    //don't call this from anywhere but the main thread
    while(readyQueueLength() > 0){
        TCB* toDelete = getReadyQueueHead();
        setReadyQueueHead(getReadyQueueHead()->next);
        free(toDelete->stackPtr);
        free(toDelete);
    }
}

void printTCB(TCB* tcb){
    printf("\nTCB: 0x%lx\n", (long) tcb);
    printf(" - rsp: 0x%lx\n", tcb->rsp);
    printf(" - rdi: 0x%lx\n", tcb->rdi);
    printf(" - rsi: 0x%lx\n", tcb->rsi);
    printf(" - rbx: 0x%lx\n", tcb->rbx);
    printf(" - r12: 0x%lx\n", tcb->r12);
    printf(" - r13: 0x%lx\n", tcb->r13);
    printf(" - r14: 0x%lx\n", tcb->r14);
    printf(" - r15: 0x%lx\n", tcb->r15);
    printf(" - next: 0x%lx\n", (long) tcb->next);
    printf(" - stackPtr: 0x%lx\n", (long) tcb->stackPtr);
    printf(" - heldMutex: 0x%lx\n", (long) tcb->heldMutex);
}

void printReadyQueue(int withDetails){
    printf("Ready Queue: ");
    int i = readyQueueLength();
    TCB* curTCB = getReadyQueueHead();
    for(int j = 0; j < i || curTCB != NULL; j++){
        printf("%ld -> ", (long) curTCB);
        if (withDetails == 1){
            printTCB(curTCB);
        }
        curTCB = curTCB->next;
    }
    printf("\n");
}

void printTCBStack(TCB* tcb, long x){
    //prints the first x entries in tcb's stack
    printf("\nTCB-%ld's stack:\n", (long) tcb);
    int i = 0;
    while(i < x){
        printf("Addr: %ld, Value: %ld\n", (long) tcb->stackPtr+i, (long) (*(tcb->stackPtr+i)));
        i++;
    }
}


// various work functions for testing different scenarios----------------------------------------------
void simpleWorkFunc(){
    //just something simple for now
    printf("working...\n");
    //printReadyQueue();
}

void singleChildYieldsBeforeCompleting() {
    printf("started test singleChildYieldsBeforeCompleting\n");
    printReadyQueue(1);
    //printTCB(getReadyQueueHead());
    //printTCB(getReadyQueueHead()->next);
    thread_yield();
    printf("back to child now\n");
    thread_yield();
    printf("last line of child thread\n");
}

//test runner-------------------------------------
int main(int argc, char** argv){
    // printf("sizeof long: %ld bytes\n", sizeof(long));
    // //test creating some threads 
    // // - TCB for main thread should be created
    // // - threads should be added to ready queue
    // printf("Ready queue length: %d - Expected: 0\n", readyQueueLength());
    // long tid1 = thread_create(simpleWorkFunc, NULL);
    // printf("tail = tid1: %s\n", tid1 == (long) getReadyQueueTail() ? "true" : "false");
    // printf("Ready queue length: %d - Expected: 2\n", readyQueueLength());
    // long tid2 = thread_create(simpleWorkFunc, NULL);
    // printf("tail = tid2: %s\n", tid2 == (long) getReadyQueueTail() ? "true" : "false");
    // printf("Ready queue length: %d - Expected: 3\n", readyQueueLength());

    // //cleanup threads before next test
    // deleteAllThreads();
    // printf("----------------------------------\n");

    // //tests around thread yield
    // // - test that first call to thread yield creates TCB for main thread
    // // - test that if there's only 1 thread it just continues on yield
    // printf("Ready queue length: %d - Expected: 0\n", readyQueueLength());
    // thread_yield();
    // printf("Ready queue length: %d - Expected: 1\n", readyQueueLength());
    // thread_yield();
    // printf("Ready queue length: %d - Expected: 1\n", readyQueueLength());

    // //cleanup threads before next test
    // deleteAllThreads();
    // printf("----------------------------------\n");

    // //test creating a single child thread which the main thread yields to. 
    // // the child thread should complete and return priority to the parent thread
    // long tid3 = thread_create(simpleWorkFunc, NULL);
    // printf("Ready queue length: %d - Expected: 2\n", readyQueueLength());
    // //printTCB(getReadyQueueHead());
    // //printTCBStack(getReadyQueueHead(), 3);
    // //printTCB(getReadyQueueHead()->next);
    // //printTCBStack(getReadyQueueHead()->next, 3);
    // printReadyQueue();
    // thread_yield();
    // printf("child thread complete, execution returned to main thread\n");
    // printReadyQueue();

    // //cleanup threads before next test
    // deleteAllThreads();
    // printf("----------------------------------\n");

    long tid4 = thread_create(singleChildYieldsBeforeCompleting, NULL);
    long tid5 = thread_create(singleChildYieldsBeforeCompleting, NULL);
    printReadyQueue(1);
    //printTCB(getReadyQueueHead());
    //printTCB((TCB*) tid4);
    thread_yield();
    printf("child thread yielded back\n");
    printReadyQueue(1);
    thread_yield();
    printf("another yield\n");
    thread_yield();
    printf("child thread complete, execution returned to main thread\n");
}