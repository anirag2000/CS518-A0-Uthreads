// File:	mypthread.h

// List all group members' names: Anirudh Srinivasa Raghavan and Ashwini Desai
// iLab machine tested on: cp.cs.rutgers.edu

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* in order to use the built-in Linux pthread library as a control for benchmarking, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

/* include lib header files that you need here: */
#include <ucontext.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stddef.h>



	/* add important states in a thread control block */

//status enumeration
typedef enum status {
	NEW,
	READY, //ready to be scheduled
	RUNNING, //thread is running
	WAITING, //waiting to be scheduled
	TERMINATED, //not running anymore
	YIELD //yeilding to another thread
} status;

//thread struct:
typedef struct mypthread_t {
	ucontext_t uco; //ucontext object
	struct mypthread_t *next_thread;
	//struct my_pthread_t * next_thread; //next thread struct pointer
	status thread_status; //thread status 
	int thread_id; //thread identification
	int num_of_runs; //number of runs

	//int time_of_runs; 
	int priority; 
	void * retval; //return value
} mypthread_t;

//queue struct
typedef struct {
	
	mypthread_t *head;
	mypthread_t *tail;
	int size;
	
} queue;

/* mutex struct definition */
//mutex struct
typedef struct {
	int flag; //flag to keep track of mutex status, 0 for unlocked, 1 for locked               
	queue*  waitq;
}mypthread_mutex_t; //fixed typedef of the struct KP




// Feel free to add your own auxiliary data structures (linked list or queue etc...)


//scheduler struct
typedef struct {
	queue * pq; //priority queue

	mypthread_t * thread_main; //main context
	mypthread_t * thread_cur; //current context
	int priority_list[16]; //priority list
	long int num_of_scheded; //counter for assigned threads

} sched;


//declarations for scheduler function calls
static void schedule();

static void sched_MLFQ();
static void sched_RR();
 sigset_t* fire_alarm();
void run_thread(mypthread_t * thr_node, void *(*f)(void *), void * arg); //to run thread using parameters from arg
void MLFQ_init(); //initiate scheduler
void sched_add(mypthread_t * thr_node, int priority); //add to the scheduler
mypthread_t * sched_choose(); //for choosing thread
void timer_init();
void sighandler();
void blankFunction ();
 mypthread_t *remove_Queue(queue *Current_Queue);

mypthread_t * sched_chooseRR();
int isNull(mypthread_t *thread);




//my_pthread_attr_t struct
typedef struct {
	char *attr1;	
} my_pthread_attr_t; //fixed the definition of typedef struct KP

/* Function Declarations: */

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg);

/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield();

/* terminate a thread */
void mypthread_exit(void *value_ptr);

/* wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr);

/* initialize a mutex */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

/* aquire a mutex (lock) */
int mypthread_mutex_lock(mypthread_mutex_t *mutex);

/* release a mutex (unlock) */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);

/* destroy a mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex);

#ifdef USE_MYTHREAD
#define pthread_t mypthread_t
#define pthread_mutex_t mypthread_mutex_t
#define pthread_create mypthread_create
#define pthread_exit mypthread_exit
#define pthread_join mypthread_join
#define pthread_mutex_init mypthread_mutex_init
#define pthread_mutex_lock mypthread_mutex_lock
#define pthread_mutex_unlock mypthread_mutex_unlock
#define pthread_mutex_destroy mypthread_mutex_destroy
#endif



#endif
