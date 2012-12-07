/** @file sched.c
 * 
 * @brief Top level implementation of the scheduler.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <config.h>
#include "sched_i.h"
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */

void sched_init(task_t* main_task  __attribute__((unused)))
{
	
}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */
 
static void __attribute__((unused)) idle(void)
{
	printf("IN IDLE!\n");
	 enable_interrupts();
	 while(1);
}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */
void allocate_tasks(task_t** tasks  __attribute__((unused)), size_t num_tasks __attribute__((unused)))
{
	uint8_t i;
	tcb_t* tcb;
	size_t num = num_tasks;
	task_t** tTasks = tasks;


	runqueue_init();


	for(i = 0; i < num; i++){
		tcb = &system_tcb[i];
		tcb->native_prio = i;
		tcb->cur_prio = i;
		tcb->holds_lock = 0;
		tcb->sleep_queue = (tcb_t*) 0;
		tcb->context.r4 = (uint32_t)idle;
		tcb->context.r5 = (uint32_t)(*tTasks)[i].data;
		tcb->context.r6 = (uint32_t)(*tTasks)[i].stack_pos;
		tcb->context.lr = (void*)launch_task;
		runqueue_add(tcb, i);
	}


	//set up idle tcb
	tcb = &system_tcb[OS_MAX_TASKS - 1];
	tcb->native_prio = OS_MAX_TASKS - 1;
	tcb->cur_prio = OS_MAX_TASKS - 1;
	tcb->holds_lock = 0;
	tcb->sleep_queue = (tcb_t*) 0;
	tcb->context.r4 = (uint32_t)idle;
	tcb->context.lr = (void*)launch_task;


	//add idle task
	dispatch_init(tcb);

	//call dispatch_nosave
	dispatch_nosave();
	
}

