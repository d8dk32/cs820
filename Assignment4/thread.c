#include "thread.h"

extern void asm_yield(TCB* curTCB, TCB* nextTCB);
extern void thread_start(void (*work)(void*), void* arg);

TCB* readyQueueHead;

long thread_create(void (*work)(void*) , void* arg){

}

void thread_yield(void){

}

int main(int argc, char** argv){

}