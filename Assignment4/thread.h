// interface for threads library
//
// 
//

//including mutex type in initial submission because the TCB struct has
// a mutex pointer that I didn't want to add later
typedef struct thread_mutex_t thread_mutex_t;
typedef struct TCB TCB;

struct thread_mutex_t{
    int locked; //boolean
    TCB* owner;
}; 

struct TCB {
    long rsp;
    long rdi;
    long rsi;
    long rbx;
    long r12;
    long r13;
    long r14;
    long r15;
    long* stackPtr; // something to always hold the bottom of the stack. Feels like it might be handy
    TCB* next;
    thread_mutex_t* heldMutex;
};

// these are the only ones that are *needed* for Part 1 
long thread_create(void (*)(void*), void*);
void thread_yield(void);

// The primitives for Part 2.
long thread_self(void); //I did this one already cause it was really simple.

// Some non-primitives that are useful to expose, for the purposes of my testing
TCB* getReadyQueueTail(void);
TCB* getReadyQueueHead(void);
void setReadyQueueHead(TCB*);
int readyQueueLength(void);
