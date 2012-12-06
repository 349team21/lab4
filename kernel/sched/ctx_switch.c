/** @file ctx_switch.c
 * 
 * @brief C wrappers around assembly context switch routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 */
 

#include <types.h>
#include <assert.h>

#include <config.h>
#include <kernel.h>
#include "sched_i.h"
#include "run_queue_i.h"

#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

static __attribute__((unused)) tcb_t* cur_tcb; /* use this if needed */

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
void dispatch_init(tcb_t* idle __attribute__((unused)))
{
	cur_tcb = idle;
	runqueue_add(cur_tcb, OS_MAX_TASKS - 1); //lowest priority for idle task
}


/**
 * @brief Context switch to the highest priority task while saving off the 
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
	uint8_t hi_prio;
	hi_prio = highest_prio();
	
	tcb_t* hi_tcb;
	hi_tcb = run_list[hi_prio];
	
	if (hi_tcb == cur_tcb) return;

	tcb_t* prev;
	prev = cur_tcb;
	cur_tcb = hi_tcb;

	ctx_switch_full(&(cur_tcb->context), &(prev->context));

}

/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
	uint8_t hi_prio;
	hi_prio = highest_prio();
	
	tcb_t* hi_tcb;
	hi_tcb = run_list[hi_prio];
	
	if (hi_tcb == cur_tcb) return;

	cur_tcb = hi_tcb;

	ctx_switch_half(&(cur_tcb->context));

}


/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
	
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
	uint8_t prio;
	prio = cur_tcb->cur_prio;
	return prio;
}

/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
	return cur_tcb;
}
