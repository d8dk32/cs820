// interface for threads library
//
// 
//

//including mutex type in initial submission because the TCB struct has
// a mutex pointer that I didn't want to add later
typedef struct thread_mutex_t thread_mutex_t;
typedef struct TCB TCB;
typedef struct thread_cond_t thread_cond_t;

struct thread_mutex_t{
    TCB* owner;
    TCB* waitList;
    thread_mutex_t* nextMutex; //there isn't really any kind of a sequence of mutexes, just making a linked list of them so I don't lose any
}; 

struct thread_cond_t {
    TCB* waitingThread;
    thread_cond_t* nextCond; ////there isn't really any kind of a sequence of mutexes, just making a linked list of them so I don't lose any
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
    long* stackPtr; //this was supposed to serve the purpose of stackBottom but I had to do some finagling with creating the stack and here we are. Whatever works.
    long* stackBottom; // something to always hold the bottom of the stack. Needed to 'free' at the end
    TCB* next;
    TCB* waitingThread; //thread waiting (joined) on this thread
    thread_mutex_t* condVarMutex;
};

// these are the only ones that are *needed* for Part 1 
long thread_create(void (*)(void*), void*);
void thread_yield(void);

// The primitives for Part 2.
long thread_self(void); //I did this one already cause it was really simple.
int thread_join(long);

int thread_mutex_init(thread_mutex_t *mutex);
int thread_mutex_lock(thread_mutex_t *mutex);
int thread_mutex_unlock(thread_mutex_t *mutex);

int thread_cond_init(thread_cond_t *cond);
int thread_cond_wait(thread_cond_t *cond, thread_mutex_t *mutex);
int thread_cond_signal(thread_cond_t *cond);


// Some utilities that I thought might be useful to expose, for the purposes of my testing
TCB* getReadyQueueTail(void);
TCB* getReadyQueueHead(void);
void setReadyQueueHead(TCB*);
int readyQueueLength(void);
int isThreadAlive(TCB*);
thread_mutex_t* getMutexListTail(void);
thread_cond_t* getCondListTail(void);
TCB* getMutexWaitListTail(thread_mutex_t* mutex);
TCB* getCondWaitListTail(thread_cond_t* cond);
TCB* getJoinWaitListHead(void);
TCB* getJoinWaitListTail(void);


