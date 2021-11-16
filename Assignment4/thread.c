#include <stdlib.h>
#include <stdio.h>
#include "thread.h"


extern void asm_yield(TCB* curTCB, TCB* nextTCB);
extern void asm_yield_cleanup(TCB* curTCB, TCB* nextTCB);

TCB* readyQueueHead = NULL;

TCB* waitingForCleanup = NULL;

TCB* joiningList = NULL; //a list of threads that have been removed from the ready queue via join and are waiting to be awoken

thread_mutex_t* mutexListHead = NULL;

thread_cond_t* condListHead = NULL;

TCB* getReadyQueueTail(){
    if (readyQueueHead == NULL) return NULL;
    TCB* curTCB = readyQueueHead;
    while(curTCB->next != NULL) {
        curTCB = curTCB->next;
    }
    return curTCB;
}

TCB* getJoinWaitListHead() {
    return joiningList;
}

TCB* getJoinWaitListTail() {
    if (joiningList == NULL) return NULL;
    TCB* curTCB = joiningList;
    while(curTCB->next != NULL) {
        curTCB = curTCB->next;
    }
    return curTCB;
}

thread_mutex_t* getMutexListTail(){
    if (mutexListHead == NULL) return NULL;
    thread_mutex_t* curMutex = mutexListHead;
    while (curMutex->nextMutex != NULL) {
        curMutex = curMutex->nextMutex;
    }
    return curMutex;
}

thread_cond_t* getCondListTail(){
    if (condListHead == NULL) return NULL;
    thread_cond_t* curCond = condListHead;
    while (curCond->nextCond != NULL) {
        curCond = curCond->nextCond;
    }
    return curCond;
}

TCB* getMutexWaitListTail(thread_mutex_t* mutex){
    if (mutex->waitList == NULL) return NULL;
    TCB* curTCB = mutex->waitList;
    while(curTCB->next != NULL) {
        curTCB = curTCB->next;
    }
    return curTCB;
}

TCB* getCondWaitListTail(thread_cond_t* cond){
    if (cond->waitingThread == NULL) return NULL;
    TCB* curTCB = cond->waitingThread;
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

int isThreadAlive(TCB* tid){
    //search the ready queue for the thread
    if (readyQueueHead == NULL) return 0;
    TCB* curTCB = readyQueueHead;
    while(curTCB != NULL){
        if (curTCB == tid) return 1;
        curTCB = curTCB->next;
    }
    //search the join waitlist
    if (joiningList != NULL){
        TCB* curTCB = joiningList;
        while (curTCB != NULL){
            if (curTCB == tid) return 1;
            curTCB = curTCB->next;
        }
    }
    //search the wait queues of each mutex
    if (mutexListHead != NULL){
        thread_mutex_t* curMutex = mutexListHead;
        while (curMutex != NULL) {
            TCB* curTCB = curMutex->waitList;
            while(curTCB != NULL){
                if (curTCB == tid) return 1;
                curTCB = curTCB->next;
            }
            curMutex = curMutex->nextMutex;
        }
    }
    //search the wait queues of each cond
    if (condListHead != NULL){
        thread_cond_t* curCond = condListHead;
        while (curCond != NULL) {
            TCB* curTCB = curCond->waitingThread;
            while(curTCB != NULL){
                if (curTCB == tid) return 1;
                curTCB = curTCB->next;
            }
            curCond = curCond->nextCond;
        }
    }
    
    return 0;
}

void wakeupThread(TCB* t){
    //wakes up the specified thread, i.e. removes it from the waiting-list and appends it to the ready queue
    //should do nothing if the thread doesn't exist
    if (t == NULL) return;

    if (joiningList == t){
        getReadyQueueTail()->next = t;
        joiningList = t->next;
        t->next = NULL;
    } else {
        TCB* curTCB = joiningList;
        while(curTCB->next != t){
            curTCB = curTCB->next;
        }
        TCB* awokenTCB = curTCB->next;
        if (awokenTCB != NULL){
            curTCB->next = awokenTCB->next;
            getReadyQueueTail()->next = awokenTCB;
            awokenTCB->next = NULL;
        }
    }
}

//the secret primitives - cleanup and threadStart
void cleanup(){
    //chop the to-be-cleaned-up TCB out of the ready queue
    //printf("made it to cleanup\n");
    TCB* cleanupTCB = readyQueueHead;
    wakeupThread(cleanupTCB->waitingThread);
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

void createMainThreadIfNeeded(){
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
        readyQueueHead->waitingThread = NULL;
        readyQueueHead->condVarMutex = NULL;
    }
}

long thread_create(void (*work)(void*) , void* arg){

    createMainThreadIfNeeded();
    
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
    newTCB->waitingThread = NULL;
    newTCB->condVarMutex = NULL;

    getReadyQueueTail()->next = newTCB;    

    return (long) newTCB;
}

void thread_yield(void){
    
    createMainThreadIfNeeded();

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
    createMainThreadIfNeeded();
    return (long) readyQueueHead;
}

int thread_join(long tid){
    
    createMainThreadIfNeeded();

    TCB* t = (TCB*) tid;

    //Error Checking
    if (t == NULL || isThreadAlive(t) == 0) return -3; //can't join a dead thread
    if (tid == thread_self()) return -1; //can't join yourself
    if ( ((TCB*) thread_self())->waitingThread == t) return -1; // prevent simple circular join
    if ( t->waitingThread != NULL) return -2; //"• -2: Another thread is already waiting to join with this thread."
    if ( readyQueueLength() == 1) return -1; //prevent another type of deadlock - all threads trying to join on another

    //the meat of Join
    TCB* runningThread = readyQueueHead;
    readyQueueHead = readyQueueHead->next;
    runningThread->next = NULL;
    t->waitingThread = runningThread;
    if(joiningList == NULL) {
        joiningList = runningThread;
    } else {
        getJoinWaitListTail()->next = runningThread;
    }
    asm_yield(runningThread, readyQueueHead);

    return 0;

}

int thread_mutex_init(thread_mutex_t *mutex){

    createMainThreadIfNeeded();

    if (mutex == NULL) return 0;

    mutex->nextMutex = NULL;
    mutex->owner = NULL;
    mutex->waitList = NULL;

    if (mutexListHead == NULL){
        mutexListHead = mutex;
    } else {
        getMutexListTail()->nextMutex = mutex;
    }

    return 1;
}

int thread_mutex_lock(thread_mutex_t *mutex){
    createMainThreadIfNeeded();
    if (mutex == NULL) return 0;
    if ((long) mutex->owner == thread_self()) return 0;

    if (mutex->owner == NULL){
        mutex->owner = readyQueueHead;
        return 1;
    } else {
        TCB* blockedTCB = readyQueueHead;
        readyQueueHead = readyQueueHead->next;
        blockedTCB->next = NULL;
        if (mutex->waitList == NULL){
            mutex->waitList = blockedTCB;
        } else {
            getMutexWaitListTail(mutex)->next = blockedTCB;
        }
        asm_yield(blockedTCB, readyQueueHead);
        return 1;
    }
}

int thread_mutex_unlock(thread_mutex_t *mutex){
    createMainThreadIfNeeded();

    if (mutex == NULL) return 0;
    if (mutex->owner == NULL) return 0;
    if (mutex->owner != (TCB*) thread_self()) return 0;

    TCB* newOwner = mutex->waitList;
    
    if (newOwner != NULL) {
        mutex->waitList = mutex->waitList->next;
        mutex->owner = newOwner;
        newOwner->next = NULL;
        getReadyQueueTail()->next = newOwner;
    } else {
        mutex->owner = NULL;
        mutex->waitList = NULL;
    }
    return 1;
}

int thread_cond_init(thread_cond_t *cond){
     createMainThreadIfNeeded();

    if (cond == NULL) return 0;

    cond->nextCond = NULL;
    cond->waitingThread = NULL;

    if (condListHead == NULL){
        condListHead = cond;
    } else {
        getCondListTail()->nextCond = cond;
    }

    return 1;
}

int thread_cond_wait(thread_cond_t *cond, thread_mutex_t *mutex) {
    if (mutex == NULL || cond == NULL) return 0;
    if (thread_mutex_unlock(mutex) == 0) return 0;

    TCB* blockingThread = readyQueueHead;
    readyQueueHead = readyQueueHead->next;
    blockingThread->next = NULL;
    blockingThread->condVarMutex = mutex;

    if (cond->waitingThread == NULL) {
        cond->waitingThread = blockingThread;
    } else {
        getCondWaitListTail(cond)->next = blockingThread;
    }

    asm_yield(blockingThread, readyQueueHead);
    return 1;
}

int thread_cond_signal(thread_cond_t *cond){
    if (cond == NULL) return 0;
    if (cond->waitingThread == NULL) return 1;

    thread_mutex_t* mu = cond->waitingThread->condVarMutex;
    TCB* signalledThread = cond->waitingThread;
    cond->waitingThread = cond->waitingThread->next;
    signalledThread->next = NULL;

    if (mu->owner == NULL){
        mu->owner = signalledThread;
        getReadyQueueTail()->next = signalledThread;
    } else {
        TCB* muWaitTail = getMutexWaitListTail(mu);
        if (muWaitTail == NULL){
            mu->waitList = signalledThread;
        } else {
            muWaitTail->next = signalledThread;
        }
    }
    return 1;
}

