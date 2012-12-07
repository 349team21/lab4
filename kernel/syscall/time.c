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
#include <arm/interrupt.h>
#include <arm/reg.h>
#include <exports.h>
#include <device.h>
#include <sched.h>
#include <arm/psr.h>
#include <kernel.h>
#include <exports.h>

#include <arm/exception.h>
#include <exports.h>
#include <task.h>
#include <device.h>
#include <sched.h>

volatile size_t start_time;
volatile size_t current_time;

unsigned long time_syscall(void)
{
 	return (current_time - start_time) / (OSTMR_FREQ/1000);
}

void time_init(void)
{

	//Interrupt setup
	reg_write(INT_ICMR_ADDR, (0x1 << INT_OSTMR_0));
	reg_write(INT_ICLR_ADDR, (0x0 << INT_OSTMR_0));

	//Oscillator setup
	reg_clear(OSTMR_OIER_ADDR, OSTMR_OIER_E1 | OSTMR_OIER_E2 | OSTMR_OIER_E3);
	reg_write(OSTMR_OIER_ADDR, OSTMR_OIER_E0);

	reg_write(OSTMR_OSCR_ADDR, 0x0);
	start_time = reg_read(OSTMR_OSCR_ADDR);
	reg_write(OSTMR_OSMR_ADDR(0), start_time + (OSTMR_FREQ/100));
	
	printf("done with time init \n");
}



void irq_handler(void) {
	printf("INTERRUPTED!\n");
	disable_interrupts();
	current_time = reg_read(OSTMR_OSCR_ADDR);
	unsigned int actual_time = current_time/OSTMR_FREQ;
	dev_update(actual_time);
	reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
	reg_write(OSTMR_OSMR_ADDR(0), current_time + (OSTMR_FREQ/100));

	dispatch_save();
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

	const size_t endTime = current_time + millis * (OSTMR_FREQ/1000);
	while (current_time < endTime);
}
