/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Harry Q Bovik < PUT YOUR NAMES HERE
 *
 * 
 * @date  
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif

mutex_t gtMutex[OS_NUM_MUTEX];

// this variable is a static counter
static int mutexCounter = 0;

// initialize mutex array gtMutex[]
void mutex_init()
{
	int i;
	for(i = 0; i < OS_NUM_MUTEX; i++){
		gtMutex[i].bAvailable = FALSE;
		gtMutex[i].pHolding_Tcb = (tcb_t*) 0;
		gtMutex[i].bLock = 0;
		gtMutex[i].pSleep_queue = (tcb_t*) 0;
	}
}

// returns a  unique identifier to a mutex object that the kernel has created
int mutex_create(void)
{
	int id;
	disable_interrupts();
	if (mutexCounter >= OS_NUM_MUTEX) {
		enable_interrupts();
		return ENOMEM;
	} else {
		gtMutex[mutexCounter].bAvailable = TRUE;
		id = mutexCounter;
		mutexCounter ++;
		enable_interrupts();
		return id;
	}
}


int mutex_lock(int mutex  __attribute__((unused)))
{
	disable_interrupts();
	// invalid mutex identifier
	if (mutex < 0 || mutex >= num_mutex){
		enable_interrupts();
		return EINVAL;
	}
	
	tcb_t* cur_tcb;
	cur_tcb = get_cur_tcb;
	
	// current task is already holding the lock
	if (gtMutex[mutex].pHolding_Tcb == cur_tcb){
		enable_interrupts();
		return EDEADLOCK;
	}
	
	uint8_t cur_prio;
	cur_prio = get_cur_prio();
	
	while (!(gtMutex[mutex].bLock == 0 && gtMutex[mutex].bAvailable == TRUE)){
		// put current task to sleep until it is acquirable
		runqueue_remove(cur_prio); // remove current task from run queue first
		
		// put the current task into sleep queue
		// case 1: no other task is waiting for this mutex
		if (gtMutex[mutex].pSleep_queue == (tcb_t*) 0){
			gtMutex[mutex].pSleep_queue = cur_tcb;
			cur_tcb.sleep_queue = (tcb_t*) 0; // not sure
		} 
		// case 2: there exists other tasks waiting for this mutex
		else {
			// if sleep queue tcb has lower priority than current priority
			if(gtMutex[mutex].pSleep_queue.cur_prio > cur_prio){
				// add it to the sleep queue of current tcb
				cur_tcb.sleep_queue = gtMutex[mutex].pSleep_queue;
				gtMutex[mutex].pSleep_queue = cur_tcb;
			}
			// if sleep queue tcb has higher priority then find the first tcb
			// in the queue that has lower priority than current priority
			else{
				tcb_t* tmp_tcb;
				tmp_tcb = gtMutex[mutex].pSleep_queue;
				tcb_t* prev;
				prev = tmp_tcb;
				while ((tmp_tcb != (tcb_t *) 0) && ((tmp_tcb.cur_prio) < cur_prio)){
					prev = tmp_tcb;
					tmp_tcb = tmp_tcb.sleep_queue;
				}
				// at this point tmp_tcb is either null or the first task has lower priority than current priority
				cur_tcb.sleep_queue = tmp_tcb;
				prev.sleep_queue = cur_tcb;
			}
		}		
		// disable interrupts externally before any context switch
		disable_interrupts();
		dispatch_save(void);
	}
	
	// ensure interrupt is disabled, not sure this line is needed
	disable_interrupts();
	cur_tcb.holds_lock = 1;
	gtMutex[mutex].pHolding_Tcb = cur_tcb;
	gtMutex[mutex].bLock = 1;
	enable_interrupts();
	return 0;
}

int mutex_unlock(int mutex  __attribute__((unused)))
{
	disable_interrupts();
	// invalid mutex identifier
	if (mutex < 0 || mutex >= num_mutex){
		enable_interrupts();
		return EINVAL;
	}

	tcb_t* cur_tcb;
	cur_tcb = get_cur_tcb();
	// current task does not hold the lock
	if (gtMutex[mutex].pHolding_Tcb != cur_tcb){
		enable_interrupts();
		return EPERM;
	}
		
	gtMutex[mutex].bAvailable = TRUE;
	gtMutex[mutex].bLock = 0;
	gtMutex[mutex].pHolding_Tcb = (tcb_t*) 0;
	
	// wake up the first task in the sleep_queue of this mutex if not null
	// then add to the run queues
	if (gtMutex[mutex].pSleep_queue != (tcb_t*) 0){
		runqueue_add(gtMutex[mutex].pSleep_queue, gtMutex[mutex].pSleep_queue.cur_prio);
		gtMutex[mutex].pSleep_queue = gtMutex[mutex].pSleep_queue.sleep_queue;
	}
	enable_interrupts();
	return 0;
}

