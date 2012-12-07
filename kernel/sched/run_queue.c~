/** @file run_queue.c
 * 
 * @brief Run queue maintainence routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-21
 */

#include "run_queue_i.h"


/**
 * @brief Clears the run-queues and sets them all to empty.
 */
void runqueue_init(void)
{
	group_run_bits = 0;
	unsigned int i;
	// we know there are 8 groups
	for (i = 0; i < 8; i++) {
		run_bits[i] = 0;
	}
	for (i = 0; i < OS_MAX_TASKS; i++) {
		run_list[i] = (tcb_t*) 0;
	}
}

/**
 * @brief Adds the thread identified by the given TCB to the runqueue at
 * a given priority.
 *
 * The native priority of the thread need not be the specified priority.  The
 * only requirement is that the run queue for that priority is empty.  This
 * function needs to be externally synchronized.
 */
void runqueue_add(tcb_t* tcb  __attribute__((unused)), uint8_t prio  __attribute__((unused)))
{
	// under the presumption that priority is unique for each tcb
	// update the run list
	run_list[prio] = tcb;

	
	// update group_run_bits
	uint8_t y;
	y = prio >> 3;
	group_run_bits = group_run_bits | (0x01 << y);
	
	// update run_bits[]
	uint8_t x;
	x = prio & 0x07;
	run_bits[y] = run_bits[y] | (0x01 << x);
}


/**
 * @brief Empty the run queue of the given priority.
 *
 * @return  The tcb at enqueued at the given priority.
 *
 * This function needs to be externally synchronized.
 */
tcb_t* runqueue_remove(uint8_t prio  __attribute__((unused)))
{
	// update the run_list
	tcb_t* tcb = run_list[prio];
	run_list[prio] = (tcb_t*) 0;


	// update group_run_bits
	uint8_t y;
	y = prio >> 3;
	group_run_bits = (group_run_bits) & (~(0x01 << y));
	
	// update run_bits[]
	uint8_t x;
	x = prio & 0x07;
	run_bits[y] = run_bits[y] & (~(0x01 << x));
	
	return tcb;		
}

/**
 * @brief This function examines the run bits and the run queue and returns the
 * priority of the runnable task with the highest priority (lower number).
 */
uint8_t highest_prio(void)
{
	// from the lab 4 recitation note second last page
	uint8_t prio, x, y;
	y = prio_unmap_table[group_run_bits];
	x = prio_unmap_table[run_bits[y]];
	prio = (y << 3) + x;
	return prio;
}
