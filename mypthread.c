// File:	mypthread.c

// List all group members' names:
// iLab machine tested on:

#include "mypthread.h"
#define STACKSIZE 4096 //size of each context
#define Z 0
#define LEVELS 3 //Number of levels in Multi-level priority queue
#define LIMIT -1
#define QUANTUMTIME 5000
#define INIT_VALUE -1
#define FLAG 1
static int id = Z;
int init = INIT_VALUE; 
static sched* scheduler;
struct itimerval it;//declare the timer struct
ucontext_t main_ctx;
mypthread_t Main;
mypthread_t threads[1024];






void init_stackspace(mypthread_t* thread)
{
 		thread->uco.uc_stack.ss_size = STACKSIZE;
        thread->uco.uc_stack.ss_sp = malloc(STACKSIZE);
        thread->uco.uc_link = &main_ctx; //so if this thread finishes, resume main
        thread->thread_id =thread->thread_id+ 1;
}

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg)
{

   if (init == LIMIT) { 
                sched_init(); 
                init = Z;
                timer_init();
        }

    if(getcontext(&(thread->uco)) == INIT_VALUE) { 
                printf("getcontext error\n");
                return -1;
        }
      init_stackspace(thread);
       
   
        makecontext(&(thread->uco), (void *)run_thread, 3, thread, function, arg);
    
      
		
				setStatus(thread, READY);
    thread->priority =Z;
    insert_queue(&(scheduler->pq[Z]), thread);
        scheduler->thread_cur = NULL;
        schedule();
        return 0; 




};


void setStatus(mypthread_t* thread, status flag)
{
	if(thread!=NULL)
	{
		thread-> thread_status=flag;
	}
}

void timer_init(){
	signal(SIGALRM, schedule);

	it.it_interval.tv_usec = QUANTUMTIME;

	it.it_value.tv_usec = QUANTUMTIME; 
	setitimer(ITIMER_REAL, &it, NULL); 
}



/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield()
{
	setStatus(scheduler->thread_cur, YIELD);
    schedule();
	return 0;
};

/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr)
{
	while(thread.thread_status == TERMINATED){
		break;
	}
	mypthread_yield();
	thread.retval = value_ptr;
	free(thread.uco.uc_stack.ss_sp); 
    return 0;
};



/* terminate a thread */
void mypthread_exit(void *value_ptr)
{
	mypthread_t* cur_thrd=scheduler->thread_cur;
    setStatus(cur_thrd, TERMINATED);
    cur_thrd->retval = value_ptr;
    swapcontext(&scheduler->thread_cur->uco, &Main.uco);
};




/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
	if (mutex==NULL)
	{
		return EINVAL;
	}
	mutex->flag=Z;
	return 0;
};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex)
{
	if (mutex==NULL)
	{
		mutex = malloc(sizeof(mypthread_mutex_t));
		mutex->waitq=malloc(sizeof(queue));
	}
	while(__sync_lock_test_and_set(&(mutex->flag), 1))
	mypthread_yield();
	return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex)
{
	if (mutex==NULL)
	{
		return EINVAL;
	}
	__sync_synchronize();
	mutex->flag=Z;
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex)
{
		if (mutex==NULL)
	{
		return EINVAL;
	}
	switch(mutex-> flag)
	{
		case 1:
		return EBUSY;
		break;
		case 0:
	
		mutex=NULL;
	}


};

/* scheduler */
static void schedule()
{


	#ifdef MLFQ
		sched_MLFQ();
	#else 
		#ifdef PSJF
		sched_PSJF();
		#else
			sched_RR();
		#endif
	#endif
	return;
}

mypthread_t * sched_chooseRR(){
	
	return remove_Queue(&(scheduler->pq[0]));

	
 
}

/* Round Robin scheduling algorithm */
static void sched_RR()
{
	printf("Calling RR");
	sigset_t* mask= fire_alarm();
	// YOUR CODE HERE
	mypthread_t * temp = scheduler->thread_cur;
	if(!isNull(temp)){

		switch(temp->thread_status)
		{
			case YIELD:
			temp->thread_status = READY;
         //   temp->priority = temp->priority;
            insert_queue(&(scheduler->pq[temp->priority]), temp);
	        break;
		}
	}
	scheduler->thread_cur = sched_chooseRR();
	
	if(isNull(scheduler->thread_cur)){
		run_thread(&Main, NULL, NULL);
	}

	setStatus(scheduler->thread_cur, RUNNING);
	//now swap contexts
	if(isNull(temp)){
		swapcontext(&main_ctx, &(scheduler->thread_cur->uco));
	}
	else {
		
		swapcontext(&(temp->uco), &(scheduler->thread_cur->uco));
	}
	sigemptyset(&mask);
	

	return;
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF()
{
// {
// sigset_t* mask= fire_alarm();
// 	mypthread_t * temp = scheduler->thread_cur;
// 	if(!isNull(temp)){

// 		switch(temp->thread_status)
// 		{
// 			case YIELD:
// 			 temp->thread_status = READY;
// 			 temp->priority=0;
//     insert_queue(&(scheduler->pq[0]), temp);
// 	break;


		

// 	default:
// 	break;
// 		}

// 	}

// 	scheduler->thread_cur = sched_choose();
	
// 	if(isNull(scheduler->thread_cur)){
// 		run_thread(&Main, NULL, NULL);
// 	}

// 	setStatus(scheduler->thread_cur, RUNNING);
// 	//now swap contexts
// 	if(isNull(temp)){
// 		swapcontext(&main_ctx, &(scheduler->thread_cur->uco));
// 	}
// 	else {
		
// 		swapcontext(&(temp->uco), &(scheduler->thread_cur->uco));
// 	}
// 	sigemptyset(&mask);
// 	return;

}


/* Preemptive MLFQ scheduling algorithm */
/* Graduate Students Only */
static void sched_MLFQ() {
sigset_t* mask= fire_alarm();
	mypthread_t * temp = scheduler->thread_cur;
	if(!isNull(temp)){

		switch(temp->thread_status)
		{
			case YIELD:
			 temp->thread_status = READY;
    temp->priority = temp->priority;
    insert_queue(&(scheduler->pq[temp->priority]), temp);
	break;


			case RUNNING:

			int np= (temp->priority)+FLAG > 2 ?2:  (temp->priority)+FLAG;
			
			
			    temp->thread_status = READY;
    temp->priority = np;
    insert_queue(&(scheduler->pq[np]), temp);
	break;

	case TERMINATED:
	case WAITING:
	break;
		}




	}



	scheduler->thread_cur = sched_choose();
	
	if(isNull(scheduler->thread_cur)){
		run_thread(&Main, NULL, NULL);
	}

	setStatus(scheduler->thread_cur, RUNNING);
	//now swap contexts
	if(isNull(temp)){
		swapcontext(&main_ctx, &(scheduler->thread_cur->uco));
	}
	else {
		
		swapcontext(&(temp->uco), &(scheduler->thread_cur->uco));
	}
	sigemptyset(&mask);
	return;
}

// Feel free to add any other functions you need

// YOUR CODE HERE

//******
// HElper Functions 
//*****
int Initialize_Queue(queue *Current_Queue)
{
    Current_Queue->tail = NULL;
    Current_Queue->size = Z;
    Current_Queue->head = NULL;
	return 1;
}

int isNull(mypthread_t* thread)
{
	if(thread==NULL)
	{
		return 1;

	}
	else{
		return 0;
	}
}

mypthread_t *peek_Queue(queue *Current_Queue)
{
    return Current_Queue->head;
}

int isEmpty_Queue(queue *Current_Queue)
{
    return Current_Queue->size == Z;
}

int insert_queue(queue *Current_Queue, mypthread_t *thr_node)
{
	if(Current_Queue==NULL)
	{
		return 0;
	}

	switch(Current_Queue-> size)
	{
		case 0:
		Current_Queue->head = thr_node;
        Current_Queue->tail = thr_node;
        Current_Queue->size++;
		break;

		default:
		Current_Queue->tail->next_thread = thr_node;
        Current_Queue->tail = thr_node;
        Current_Queue->size++;


	}


	return 1;
}
mypthread_t *remove_Queue(queue *Current_Queue)
{
    if (Current_Queue->size == Z)
    {
        printf("Cant Remove");
        return NULL;
    }
    mypthread_t *tmp;
	switch(Current_Queue-> size)
	{
		case 1:
		tmp = Current_Queue->head;
        Current_Queue->head = NULL;
        Current_Queue->tail = NULL;
		break;

		default:
		tmp = Current_Queue->head;
        Current_Queue->head = Current_Queue->head->next_thread;


	}

    tmp->next_thread = NULL;
    Current_Queue->size--;
    return tmp;
}



void run_thread(mypthread_t * thread_node, void *(*f)(void *), void * arg){
	if(isNull(thread_node))
	{
		return;
	}
	setStatus(thread_node, RUNNING);
	scheduler->thread_cur = thread_node;
	thread_node = f(arg);
	setStatus(thread_node, TERMINATED);
	schedule();
}
void initialize_queue( int num_levels)
{
	    scheduler->pq = (queue *)malloc(sizeof(queue)*num_levels); 
        int k;
        for(k = 0; k < LEVELS; k++){
          Initialize_Queue(&(scheduler->pq[k])); 
		}
}

int sched_init(){
        if(init != LIMIT){
			return -1;
        }
        scheduler = malloc(sizeof(sched));
        getcontext(&main_ctx); 
        Main.uco = main_ctx;
        Main.uco.uc_link = NULL;
		setStatus(&Main, READY);
        scheduler->thread_main = &Main;
		setStatus(scheduler->thread_main, READY);
        scheduler->thread_main->thread_id = 0;
        scheduler->thread_cur = NULL;
		initialize_queue(LEVELS);

        }




mypthread_t * sched_choose(){
	int chosen_priority=-1;



	for(int i = 0; i < LEVELS; i++){//three levels
		if(scheduler->pq[i].head != NULL){
			chosen_priority=i;
		}
	}
	if(chosen_priority!=-1)
	{
		return remove_Queue(&(scheduler->pq[chosen_priority]));

	}
	else{
  		 return NULL;
	}
 
}

void signal_addset(sigset_t* mask)
{
	sigaddset (&mask, SIGALRM);
	sigaddset (&mask, SIGQUIT);
}

sigset_t* fire_alarm()
{
	struct sigaction act;
	sigset_t block_mask;
	sigemptyset (&block_mask);
	signal_addset(&block_mask);
	act.sa_handler = blankFunction;
	act.sa_mask = block_mask;
	act.sa_flags = Z;
	sigaction (SIGALRM, &act, NULL);
	return &block_mask; 
}



void blankFunction()
{
	return;
}


