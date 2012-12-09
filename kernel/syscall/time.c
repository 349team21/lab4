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
#include <arm/timer.h>

extern volatile uint32_t irq_time;

#define TIMER_CLEAR 0x0
#define OSTMR_FREQ            3686400      /* Oscillator frequency in hz */


void init_timer(void) {
	reg_write(INT_ICMR_ADDR, 1<<INT_OSTMR_0);
	reg_write(OSTMR_OSMR_ADDR(0), OSTMR_FREQ/100);
	reg_write(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
	reg_write(OSTMR_OSCR_ADDR, 0x0);

}



unsigned long time_syscall(void)
{
 return (unsigned long)(irq_time*10);	
}


void irq_handler(void) {
	disable_interrupts();

	irq_time ++;
	dev_update(irq_time);
	reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
	reg_write(OSTMR_OSCR_ADDR, TIMER_CLEAR);

	/* Wait for timer to reset */
	while(((volatile unsigned)reg_read(INT_ICPR_ADDR) >>INT_OSTMR_0) & 0x1);

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
	unsigned long currTime, endTime; 
	currTime = (unsigned long)(irq_time*10);
	endTime = currTime + millis;

	/* loop while time is not up yet*/
	while((unsigned long)(irq_time*10) < endTime){;}	
}

