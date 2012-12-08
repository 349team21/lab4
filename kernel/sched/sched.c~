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

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */
void bubble_sort(task_t tasks[], size_t array_size);
void idle_task_setup(tcb_t* idle_task);
void task_setup(tcb_t* task_tcb, task_t task, unsigned prio);

extern uint32_t global_data;

static void __attribute__((unused)) idle(void);



void sched_init(task_t* main_task  __attribute__((unused)))
{
	
}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */
 
static void __attribute__((unused)) idle(void)
{
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
void allocate_tasks(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
	unsigned int i;
	bubble_sort(*tasks, num_tasks);
	
	idle_task_setup(&(system_tcb[IDLE_PRIO]));
	runqueue_add (&(system_tcb[IDLE_PRIO]),
		system_tcb[IDLE_PRIO].native_prio);
	
	for (i = 0; i < num_tasks; i++){
		task_setup (&(system_tcb[i]), (*tasks)[i], i+1);
		runqueue_add(&(system_tcb[i]), system_tcb[i].native_prio);
	}
}


void bubble_sort(task_t tasks[], size_t array_size)
{
	unsigned i, j;
	task_t temp;
 
	for (i = (array_size-1); i>0; i--){
		for (j = 1; j <= i; j++){
			if (tasks[j-1].T > tasks[j].T){
				temp = tasks[j-1];
				tasks[j-1] = tasks[j];
				tasks[j] = temp;
			}
		}
	}
}

void idle_task_setup(tcb_t* idle_task){
	idle_task->native_prio = IDLE_PRIO;
	idle_task->cur_prio = IDLE_PRIO;
	idle_task->holds_lock = 0;
	idle_task->context.r4 = (uint32_t) idle;
	idle_task->context.r5 = (uint32_t) 0;
	idle_task->context.r6 = (uint32_t) 0xa2f00000;
	idle_task->context.r7 = (uint32_t) 0;
	idle_task->context.r8 = (uint32_t) global_data;
	idle_task->context.r9 = (uint32_t) 0;
	idle_task->context.r10 = (uint32_t) 0;
	idle_task->context.r11 = (uint32_t) 0;
	idle_task->context.sp = (void*) idle_task->kstack_high;
	idle_task->context.lr = (void*) launch_task;
	idle_task->sleep_queue = (tcb_t*) 0;
}

void task_setup(tcb_t* task_tcb, task_t task, unsigned prio){
	task_tcb->native_prio = prio;
	task_tcb->cur_prio = prio;
	task_tcb->holds_lock = 0;
	task_tcb->context.r4 = (uint32_t) task.lambda;
	task_tcb->context.r5 = (uint32_t) task.data;
	task_tcb->context.r6 = (uint32_t) task.stack_pos;
	task_tcb->context.r7 = (uint32_t) 0;
	task_tcb->context.r8 = (uint32_t) global_data;
	task_tcb->context.r9 = (uint32_t) 0;
	task_tcb->context.r10 = (uint32_t) 0;
	task_tcb->context.r11 = (uint32_t) 0;
	task_tcb->context.sp = (void*) task_tcb->kstack_high;
	task_tcb->context.lr = (void*) launch_task;
	task_tcb->sleep_queue = (tcb_t*) 0;
}

