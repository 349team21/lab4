/** @file proc.c
 * 
 * @brief Implementation of `process' syscalls
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-12
 */

#include <exports.h>
#include <bits/errno.h>
#include <config.h>
#include <kernel.h>
#include <syscall.h>
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>

int task_create(task_t* tasks  __attribute__((unused)), 
		size_t num_tasks  __attribute__((unused)))
{
	disable_interrupts();

	//at most 62 tasks, one for idle, one for priority inversion
	if (num_tasks > OS_MAX_TASKS - 2)	return -EINVAL;

	if (((size_t)tasks  <  (size_t)0xa0000000) || ((size_t)tasks > (size_t)0xa3ffffff)) {
		return -EFAULT;
	}

	if(!assign_schedule(&tasks, num_tasks))
		return -ESCHED;

	runqueue_init();
	dev_init();
	allocate_tasks(&tasks, num_tasks);
	dispatch_nosave();

  	return 0xbadc0de;
}

int event_wait(unsigned int dev  __attribute__((unused)))
{
	tcb_t* cur_tcb;

	disable_interrupts();

	if (dev > 3){
		enable_interrupts();
		return EINVAL;
	}

	cur_tcb = get_cur_tcb();
	runqueue_remove(cur_tcb->cur_prio);

	dev_wait(dev);

	dispatch_save();

	return 0;
}


/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}


