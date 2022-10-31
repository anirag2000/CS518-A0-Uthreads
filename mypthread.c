#include <stdio.h>
#include <stdlib.h>

#define STACKSIZE 15000
#define GCERROR -1
#define _XOPEN_SOURCE 600
#define mutex_code long int
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

static long int thr_id = 0;
typedef struct{

} mypthread_attr_t;

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

typedef struct {

	mypthread_t *head;
	mypthread_t *tail;
	long int size;

}Queue_ds;

// pthread_mutex object
typedef struct my_pthread_mutex_t {
	volatile int guard;
	volatile int flag;
	Queue_ds *wait;
	mypthread_t owner;
}my_pthread_mutex_t;


void *print_message_function( void *ptr );
int my_pthread_create(mypthread_t *thread, mypthread_attr_t *attr, void *(*function)(void *), void *arg);
void run_thread(mypthread_t * thr_node, void *(*f)(void *), void * arg);






int main()
{
     mypthread_t thread1, thread2;
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     int  iret1, iret2;

    /* Create independent threads each of which will execute function */

     iret1 = my_pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = my_pthread_create( &thread2, NULL, print_message_function, (void*) message2);
     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

   //  pthread_join( thread1, NULL);
   //  pthread_join( thread2, NULL); 

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     return 0;
}

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
}

int my_pthread_create(mypthread_t *thread, mypthread_attr_t *attr, void *(*function)(void *), void *arg)
{
    if (getcontext(&(thread->ucp)) != 0)
    {
        printf("Get_Context error, please check ucp");
        return GCERROR;
    }

    thread->ucp.uc_stack.ss_sp = malloc(STACKSIZE);
    thread->ucp.uc_stack.ss_size = STACKSIZE;
    thread->thr_id = thr_id++;
 //   thread->start_tt = timeStamp();
    thread->first_exe_tt = 0;
 //   makecontext(&(thread->ucp), (void *)runThread, 3, thread, function, arg);
  //  schedAddThread(thread, 0);
    printf("Thread %ld successfully created\n", thr_id);
    return 0;
}













//**************************************
// HElper Functions 
//***************************************
void Initialize_Queue(Queue_ds *Current_Queue)
{
    Current_Queue->tail = NULL;
    Current_Queue->size = 0;
    Current_Queue->head = NULL;
}

mypthread_t *peek_Queue(Queue_ds *Current_Queue)
{
    return Current_Queue->head;
}

bool isEmpty_Queue(Queue_ds *Current_Queue)
{
    return Current_Queue->size == 0;
}

void insert_queue(Queue_ds *Current_Queue, mypthread_t *thr_node)
{
    if (Current_Queue->size == 0)
    {
        Current_Queue->head = thr_node;
        Current_Queue->tail = thr_node;
        Current_Queue->size++;
    }
    else
    {
        Current_Queue->tail->next_thr = thr_node;
        Current_Queue->tail = thr_node;
        Current_Queue->size++;
    }
}
mypthread_t *remove_Queue(Queue_ds *Current_Queue)
{
    if (Current_Queue->size == 0)
    {
        printf("Cant Remove");
        return NULL;
    }
    mypthread_t *tmp;
    if (Current_Queue->size == 1)
    {
        tmp = Current_Queue->head;
        Current_Queue->head = NULL;
        Current_Queue->tail = NULL;
    }
    else
    {
        tmp = Current_Queue->head;
        Current_Queue->head = Current_Queue->head->next_thr;
    }
    tmp->next_thr = NULL;
    Current_Queue->size--;
    return tmp;
}




///**************************************************************
///MUTEX FUNCTIONS
//****************************************************************
mutex_code my_pthread_mutex_init(my_pthread_mutex_t *mutex){
	if (mutex==NULL)
	{
		return EINVAL;
	}
	mutex->flag=0;
	return 0;
}

mutex_code my_pthread_mutex_lock(my_pthread_mutex_t *mutex){
	if (mutex==NULL)
	{
		return EINVAL;
	}
	while(__sync_lock_test_and_set(&(mutex->flag), 1))
	// my_pthread_yield();
	return 0;
}

mutex_code my_pthread_mutex_unlock(my_pthread_mutex_t *mutex){
	if (mutex==NULL)
	{
		return EINVAL;
	}
	__sync_synchronize();
	mutex->flag=0;
	return 0;
}

mutex_code my_pthread_mutex_destroy(my_pthread_mutex_t *mutex){
	// if (mutex->flag==1)
	// {
	// 	return EBUSY;
	// }

	switch(mutex-> flag)
	{
		case 1:
		return EBUSY;
		break;
	}
	// if (mutex->flag==0)
	// {
	// 	free(mutex);
	// 	mutex=NULL;
	// }

	switch(mutex-> flag)
	{
		case 0:
		free(mutex);
		mutex=NULL;
	}
	return 0;
}


