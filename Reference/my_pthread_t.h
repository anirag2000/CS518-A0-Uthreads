#define _XOPEN_SOURCE 600
#include <ucontext.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <ucontext.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_LEVELS 4


// states object
typedef enum state {
	NEW,
	READY,
	RUNNING,
	WAITING,
	TERMINATED,
	YIELD
}state;


// mythread object
typedef struct mypthread_t {
	ucontext_t ucp;
	struct mypthread_t *next_thr;
	state thr_state;
	long thr_id;
	int num_runs;
	int time_runs;
	int priority;
	void * retval;
	long start_tt;
	long first_exe_tt;
	long last_exe_tt;
	long end_tt;
}mypthread_t;

// queue object
typedef struct {

	mypthread_t *head;
	mypthread_t *tail;
	int size;

}queue;

// scheduler object
typedef struct {
	queue *running_queue;
	queue *waiting_queue;
	mypthread_t *thr_main;
	mypthread_t *thr_cur;
	int prior_list[NUM_LEVELS];
	long num_sched;
}scheduler;

// pthread_mutex object
typedef struct my_pthread_mutex_t {
	volatile int guard;
	volatile int flag;
	queue *wait;
	mypthread_t owner;
}my_pthread_mutex_t;

typedef struct
{
}mypthread_attr_t;

// API of benchmark
void test(int cap);
void mutexTestOne();
void mutexTestTwo();
void noMutexTestOne();
void noMutexTestTwo();

// API of schdule handler
void scheduler_handler();

// API of thread
void schedInit();
void schedAddThread(mypthread_t * thr_node, int priority);
mypthread_t * sched_pickThread();
void run_thread(mypthread_t * thr_node, void *(*f)(void *), void * arg);
void aging_adjust();

// API of queue
mypthread_t *peek(queue * first);
char queue_isEmpty(queue * first);
void queue_init(queue * first);
void enqueue(queue * first, mypthread_t * thr_node);
mypthread_t *dequeue(queue * first);


// API of scheduler
long timeStamp();
void schedInit();
void schedAddThread(mypthread_t *thr_node, int priority);
mypthread_t *schedPickThread();
void runThread(mypthread_t *thr_node, void *(*f)(void *), void *arg);

// Creates a pthread that executes function. Attributes are ignored.
int my_pthread_create(mypthread_t *thread, mypthread_attr_t *attr, void *(*function)(void *), void *arg);

// Explicit call to the my_pthread_t scheduler requesting that the current context be swapped out and another be scheduled.
void my_pthread_yield();

// Explicit call to the my_pthread_t library to end the pthread that called it. If the value_ptr isn't NULL, any return value from the thread will be saved.
void my_pthread_exit(void * value_ptr);

// Call to the my_pthread_t library ensuring that the calling thread will not  execute until the one it references exits. If value_ptr is not null, the return value of the  exiting thread will be passed back.
int my_pthread_join(mypthread_t *thread, void ** value_ptr);

// Initializes a my_pthread_mutex_t created by the calling thread. Attributes are ignored.
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_attr_t *mutexattr);

// Locks a given mutex, other threads attempting to access this mutex will not run until it is unlocked.
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);

// Unlocks a given mutex.
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);

// Destroys a given mutex. Mutex should be unlocked before doing so.
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);

