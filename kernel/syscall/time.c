/** @file time.c
 *
 * @brief Kernel timer based syscall implementations
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <config.h>
#include <bits/errno.h>
#include <arm/timer.h>
#include <syscall.h>


unsigned long time_syscall(void)
{
 	return (current_time - start_time) / (OSTMR_FREQ/1000);
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 * 
 */
void sleep_syscall(unsigned long millis  __attribute__((unused)))
{
	/* Enable IRQ */
	asm("mrs r12, cpsr");
	asm("bic r12, r12, #0x80");
	asm("msr cpsr, r12");

	const size_t endTime = current_time + duration * (OSTMR_FREQ/1000);
	while (current_time < endTime);
}
