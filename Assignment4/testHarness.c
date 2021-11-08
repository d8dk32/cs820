#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

void deleteAllThreads(){
    while(readyQueueLength() > 0){
        TCB* toDelete = getReadyQueueHead();
        setReadyQueueHead(getReadyQueueHead()->next);
        free(toDelete->stackPtr);
        free(toDelete);
    }
}

void testWorkFunc(){
    //nothing
}

int main(int argc, char** argv){
    //test creating some threads 
    // - TCB for main thread should be created
    // - threads should be added to ready queue
    printf("Ready queue length: %d - Expected: 0\n", readyQueueLength());
    long tid1 = thread_create(testWorkFunc, NULL);
    printf("tail = tid1: %s\n", tid1 == (long) getReadyQueueTail() ? "true" : "false");
    printf("Ready queue length: %d - Expected: 2\n", readyQueueLength());
    long tid2 = thread_create(testWorkFunc, NULL);
    printf("tail = tid2: %s\n", tid2 == (long) getReadyQueueTail() ? "true" : "false");
    printf("Ready queue length: %d - Expected: 3\n", readyQueueLength());

    //cleanup threads before next test
    deleteAllThreads();

    //tests around thread yield
    // - test that first call to thread yield creates TCB for main thread
    // - test that if there's only 1 thread it just continues on yield
    printf("Ready queue length: %d - Expected: 0\n", readyQueueLength());
    thread_yield();
    printf("Ready queue length: %d - Expected: 1\n", readyQueueLength());
    thread_yield();
    printf("Ready queue length: %d - Expected: 1\n", readyQueueLength());

    //cleanup threads before next test
    deleteAllThreads();
}