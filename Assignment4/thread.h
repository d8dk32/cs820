// interface for threads library
//
// 
//

typedef struct thread_mutex_t thread_mutex_t;
typedef struct TCB TCB;

//including mutex type in initial submission because the TCB struct has
// a mutex pointer that I didn't want to add later
struct thread_mutex_t{
    int locked; //boolean
    TCB* owner;
}; 

struct TCB {
    long rdi;
    long rsi;
    long rsp;
    long rbx;
    long r12;
    long r13;
    long r14;
    long r15;
    TCB* next;
    long* stackPtr;
    thread_mutex_t* heldMutex;
};

long thread_create(void (*)(void*), void*);
void thread_yield(void);