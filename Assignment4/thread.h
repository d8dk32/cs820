// interface for threads library
//
// 
//

typedef struct Mutex Mutex;
typedef struct TCB TCB;

typedef struct {
    TCB* owner;
} Mutex;

typedef struct {
    long rdi;
    long rsi;
    long rsp;
    long rbx;
    long r12;
    long r13;
    long r14;
    long r15;
    TCB* next;
    int* stackPtr;
    Mutex* heldMutex;
} TCB;


long thread_create(void (*)(void*), void*);
void thread_yield(void);