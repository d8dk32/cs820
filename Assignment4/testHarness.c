#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

// some utility functions for testing---------------------------------------------------------------

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

// various work functions for testing different scenarios----------------------------------------------
void simpleWorkFunc(){
    //just something simple for now
    printf("simple work func is working...\n");
    //printReadyQueue();
}

void childSimplyYields(){
    printf("I'm simply gonna yield...\n");
    thread_yield();
    printf("back after simply yielding. Done now.\n");
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

void childThreadCreatesAnotherThread() {
    printReadyQueue(0);
    printf("This child thread is about to create a thread. Look out!\n");
    thread_create(simpleWorkFunc, NULL);
    printReadyQueue(0);
    thread_yield();
    printf("Original Child has reached its end\n");
}

void simpleFuncWithArg(void* arg) {
    int intArg = *((int*) arg);
    printf("The argument to this simple func is: %d\n", intArg);
}

void childThreadJoins(void* arg) {
    long joinTid = *((long*) arg);
    printf("This thread will now join on thread %ld\n", joinTid);
    int j = thread_join(joinTid);
    printf("Childjoiner thread is back. Join status: %d\n", j);
}

void childThreadLocksAndReleases(void* arg) {
    thread_mutex_t* mu = (thread_mutex_t*) arg;
    printf("locking on the mutex\n");
    thread_mutex_lock(mu);
    printf("got the lock\n");
    thread_mutex_unlock(mu);
    printf("released the lock\n");
}

thread_cond_t testCond1;
void childThreadWaitsForSignal(void* arg) {
    thread_mutex_t* mu = (thread_mutex_t*) arg;
    printf("thread %ld is locking on mutex %lx\n", thread_self(), (long) mu);
    thread_mutex_lock(mu);
    printf("thread %ld has the mutex, waiting for signal\n", thread_self());
    thread_cond_wait(&testCond1, mu);
    printf("thread %ld has received the signal. Unlocking...\n", thread_self());
    thread_mutex_unlock(mu);
}

thread_mutex_t deadlock1;
thread_mutex_t deadlock2;
void childThreadAimsForDeadlock(void* arg) {
    printf("child locking 2\n");
    thread_mutex_lock(&deadlock2);
    printf("child locking 1\n");
    thread_mutex_lock(&deadlock1);
}

//test runner-------------------------------------
int main(int argc, char** argv){

    if (argc != 2){
        printf("no test number specified\n");
        return -1;
    }

    int testNum = atoi(argv[1]);

    if(testNum == 1){

        printf("sizeof long: %ld bytes\n", sizeof(long));
        //test creating some threads 
        // - TCB for main thread should be created
        // - threads should be added to ready queue
        printf("Ready queue length: %d - Expected: 0\n", readyQueueLength());
        long tid1 = thread_create(simpleWorkFunc, NULL);
        printf("tail = tid1: %s\n", tid1 == (long) getReadyQueueTail() ? "true" : "false");
        printf("Ready queue length: %d - Expected: 2\n", readyQueueLength());
        long tid2 = thread_create(simpleWorkFunc, NULL);
        printf("tail = tid2: %s\n", tid2 == (long) getReadyQueueTail() ? "true" : "false");
        printf("Ready queue length: %d - Expected: 3\n", readyQueueLength());

    } else if (testNum == 2){

        //tests around thread yield
        // - test that first call to thread yield creates TCB for main thread
        // - test that if there's only 1 thread it just continues on yield
        printf("Ready queue length: %d - Expected: 0\n", readyQueueLength());
        thread_yield();
        printf("Ready queue length: %d - Expected: 1\n", readyQueueLength());
        thread_yield();
        printf("Ready queue length: %d - Expected: 1\n", readyQueueLength());

    } else if (testNum == 3) {

        //test creating a single child thread which the main thread yields to. 
        // the child thread should complete and return priority to the parent thread
        long tid3 = thread_create(simpleWorkFunc, NULL);
        printf("Ready queue length: %d - Expected: 2\n", readyQueueLength());
        //printTCB(getReadyQueueHead());
        //printTCBStack(getReadyQueueHead(), 3);
        //printTCB(getReadyQueueHead()->next);
        //printTCBStack(getReadyQueueHead()->next, 3);
        printReadyQueue(0);
        thread_yield();
        printf("child thread complete, execution returned to main thread\n");
        printReadyQueue(0);

    } else if (testNum == 4) {

        long tid4 = thread_create(singleChildYieldsBeforeCompleting, NULL);
        long tid5 = thread_create(singleChildYieldsBeforeCompleting, NULL);
        printReadyQueue(0);
        //printTCB(getReadyQueueHead());
        //printTCB((TCB*) tid4);
        thread_yield();
        printf("child thread yielded back\n");
        printReadyQueue(0);
        thread_yield();
        printf("another yield\n");
        thread_yield();
        printf("child thread complete, execution returned to main thread\n");
    } else if (testNum == 5){
        long tid6 = thread_create(childThreadCreatesAnotherThread, NULL);
        printf("This thread will have a grandchild\n");
        thread_yield();
        printf("Back to main now. The grandchild should kick off next with simpleWorkFunc\n");
        thread_yield();
        printf("And back to main again. Both child and grandchild should have ended\n");
    } else if (testNum == 6) {
        int arg1 = 1;
        int arg2 = 2;
        long tid7 = thread_create(simpleFuncWithArg, &arg1);
        long tid8 = thread_create(simpleFuncWithArg, &arg2);
        printf("Lets see if those args work\n");
        thread_yield();
        printf("Back to main. Make sure both threads printed out their args.\n");
    } else if (testNum == 7) {
        long tid9 = thread_create(simpleWorkFunc, NULL);
        printf("main thread should wait for child to finish\n");
        thread_join(tid9);
        printf("execution passed back to main\n");
        printTERs();
    } else if (testNum == 8) {
        long tid10 = thread_create(singleChildYieldsBeforeCompleting, NULL);
        long tid11 = thread_create(childThreadJoins, &tid10);
        printf("yielding to first child\n");
        thread_yield();
        printf("back to main thread. yielding again\n");
        thread_yield();
        printf("back to main once more. yielding again...\n");
        thread_yield();
        printf("first child thread should have ended and woken up 2nd child. Yield to it now...\n");
        thread_yield();
        printf("back to the main again. Test Over\n");
        printTERs();
    } else if (testNum == 9) {
        long self = thread_self();
        printf("testing some join error conditions...\n");
        int status = thread_join((long) 12345);
        printf("Join error - expected: %d, actual: %d\n", -3, status);
        status = thread_join(self);
        printf("Join error - expected: %d, actual: %d\n", -1, status);

        long tid12 = thread_create(childThreadJoins, &self);
        thread_yield();
        status = thread_join(tid12);
        printf("Join error - expected: %d, actual: %d\n", -1, status); 
        long tid13 = thread_create(childThreadJoins, &self);
        thread_yield();
        printf("child thread should have had a join error of -2\nTest over.\n");
        printTERs();
    } else if (testNum == 10) {
        thread_mutex_t mutex1, mutex2;
        int mu1 = thread_mutex_init(&mutex1);
        int mu2 = thread_mutex_init(&mutex2);
        int mu3 = thread_mutex_init(NULL);

        printf("%lx -> %lx -> %lx\n", (long) &mutex1, (long) mutex1.nextMutex, (long) mutex1.nextMutex->nextMutex);
        printf("Statuses: %d, %d, %d\n", mu1, mu2, mu3);
        printTERs();

    } else if (testNum == 11) {
        thread_mutex_t mu;
        int m1 = thread_mutex_init(&mu);
        long tid14 = thread_create(childThreadLocksAndReleases, &mu);
        thread_mutex_init(&mu);
        printf("main thread locking on mutex\n");
        int lock = thread_mutex_lock(&mu);
        printf("Lock status: %d. Yielding to child\n", lock);
        thread_yield();
        printf("back to main. releasing lock and yielding\n");
        thread_mutex_unlock(&mu);
        thread_yield();
        printf("Execution returned to main. test over\n");
        printTERs();
    } else if (testNum == 12) {
        long tid0 = thread_create(childSimplyYields, NULL);
        long tid1 = thread_create(childThreadJoins, &tid0);
        long tid2 = thread_create(childThreadJoins, &tid1);
        long tid3 = thread_create(childThreadJoins, &tid2);
        printf("tid0: %ld, tid1: %ld, tid2: %ld, tid3: %ld\n", tid0, tid1, tid2, tid3);
        int join = thread_join(tid3);
        thread_yield();
        printf("Main is back. join status: %d\n", join);
        printTERs();
    } else if (testNum == 13) {
        thread_mutex_t mu1, mu2;
        thread_mutex_init(&mu1);
        thread_mutex_init(&mu2);
        long tid1 = thread_create(childThreadLocksAndReleases, &mu1);
        long tid2 = thread_create(childThreadLocksAndReleases, &mu2);
        thread_mutex_lock(&mu1);
        thread_mutex_lock(&mu2);
        printf("first yield\n");
        thread_yield();
        thread_mutex_unlock(&mu2);
        printf("2nd yield\n");
        thread_yield();
        thread_mutex_unlock(&mu1);
        printf("3rd yield\n");
        thread_yield();
        printf("test over\n");
        printTERs();
    } else if (testNum == 14) {
        thread_mutex_t mu;
        thread_mutex_init(&mu);
        thread_cond_init(&testCond1);
        long tid1 = thread_create(childThreadWaitsForSignal, &mu);
        printf("Thread 1: %ld\n", tid1);
        thread_yield();
        long tid2 = thread_create(childThreadWaitsForSignal, &mu);
        printf("Thread 2: %ld\n", tid2);
        thread_yield();
        printf("Signalling...\n");
        thread_cond_signal(&testCond1);
        printf("Signalling again...\n");
        thread_cond_signal(&testCond1);
        printf("main will now wait for the kids finish up\n");
        thread_join(tid2);
        printf("test over\n");
        printTERs();
    } else if (testNum == 15) {
        //trying to reproduce the professor's mutex deadlocking test
        thread_mutex_init(&deadlock1);
        thread_mutex_init(&deadlock2);
        printf("parent locking 1\n");
        thread_mutex_lock(&deadlock1);
        printf("parent creating thread 1\n");
        long tid1 = thread_create(childThreadAimsForDeadlock, NULL);
        printf("parent yielding\n");
        thread_yield();
        printf("parent locking 2\n");
        thread_mutex_lock(&deadlock2);
        printf("now what?\n");
        printTERs();

    }
    
    else {
        printf("test number %d does not exist\n", testNum);
    }


}