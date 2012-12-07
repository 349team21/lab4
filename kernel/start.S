/**
 * @file  start.S
 *
 * @brief Kernel startup routine
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Tue, 23 Oct 2007 11:20:33 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-07-06
 */

#include <asm.h>
#include <arm/psr.h>
#define SWI_IMM_MASK 0x00ffffff
#define CPSR_IRQ_FLAG  0x80
#define CPSR_FIQ_FLAG  0x40
#define CPSR_USR_MODE  0x10
#define ENABLE_IRQ   0x53
#define	DISABLE_IRQ  0xD3

#define LOAD_ADDR 0xa0000000
#define USR_STACK 0xa3000000
#define IRQ_STACK 0xa2000000

	.file   "start.S"
	.text
	.global handleSWI
	.type handleSWI, %function
	.global handleIRQ
	.type handleIRQ, %function
	.global toUSER
	.type toUSER, %function
	
	/*
	 * Branch immediately to main without altering the execution environment.
	 * This indirection is needed to guarantee that the program entry point is
	 * LOAD_ADDR (probably 0xa3000000) without placing strict
	 * requirements on the layout of kernel.c.  Bit first, provide main with the
	 * jump table address as one of its arguments.
	 */
FUNC(_start)
	mov     r2, r8
	b       kmain

handleSWI:
	//save r0 to r12, sp and lr, in total of 15 registers
	stmfd	sp, {r0-r12, sp, lr}
	sub		sp, sp, #60
	
	// save spsr
	mrs      r12, spsr
	stmfd    sp!, {r12}

	@ Restore jump table in r8.
	ldr	ip, =global_data
	ldr	r8, [ip]

	@ Determine the SWI number and call the dispatch function.
	ldr	r0, [lr, #-4]
	bic	r0, r0, #~SWI_IMM_MASK
	mov	r1, sp
	add r1, r1, #4
	
	@ Enable IRQs
	@msr	cpsr_c, #ENABLE_IRQ
	
	bl	my_swi_dispatcher

	@ Disable IRQs
	msr	cpsr_c, #DISABLE_IRQ
	
	@ Return to user mode.
	ldmfd    sp!, {r12}
	msr      spsr, r12
	
	add		sp, sp, #4
	ldmfd	sp!, {r1-r12, pc}^


handleIRQ:
/* lr starts off pointing at next instruction + 4 -- fix this. */
	sub      lr, lr, #4

	/* No nesting -- this is a temporary stack. */
	ldr      sp, =irq_stack_hi
	stmfd    sp!, {r0,r1}

	/* Move special regs into r0, r1. */
	mrs      r0, spsr
	mov      r1, lr

	/* Switch to supervisor mode */
	msr      cpsr_c, #(PSR_MODE_SVC | PSR_IRQ)

	/* Diq the spsr_svc because svc-mode (SWI) entry code saves it with IRQ 
	 * disabled.  I feel so dirty doing this.  
	 * We need to diq spsr because we need atomic reload of pc and cpsr
	 * during the return sequence in case we came in from user mode.
	 */
	msr      spsr, r0

	/* Save lr, pop of r0, r1 from irq stack, and then put them all on the 
	 * svc stack along with other caller-save registers.
	 * The stack will look like:
	 * {r0-r3, r8, ip, lr, pc} of source
	 */
	stmfd    sp!, {r1}
	ldr      r1, =irq_stack_lo
	ldm      r1, {r0, r1}
	stmfd    sp!, {r0-r3, r8, ip, lr}

	/* Recover user registers and save */
	mrs      r0, spsr
	stmfd    sp, {r0, sp, lr}^
	add      sp, sp, #-12

	/* Set up r8 correctly with u-boot dispatch table. */
	ldr      ip, =global_data
	ldr      r8, [ip]
	
	/* Call the IRQ handler in C. */
	bl       irq_handler
	@mov      r0, sp
	@mov      r1, #128
	@bl       hexdump

	/* Restore registers.  Also restore CPSR. */
	ldmfd    sp, {r0, sp, lr}^
	add      sp, sp, #12
	msr      spsr, r0
	ldmfd    sp!, {r0-r3, r8, ip, lr, pc}^


toUSER:	
    @ Stash regs, jump table in r8, and sp for exit_kernel.
	stmfd	sp!, {r4-r7, r9-r11, lr}
	ldr	ip, =global_data
	str	r8, [ip]
	ldr	ip, =saved_sp
	str	sp, [ip]

	@ Switch to user mode & load user stack.
	@msr	cpsr_c, #(CPSR_FIQ_FLAG |CPSR_USR_MODE)
	msr	cpsr_c, #(CPSR_USR_MODE)
	ldr	sp, =USR_STACK

	@ Push argv NULL terminator.
	mov	ip, $0
	str	ip, [sp, #-4]!

	@ Calculate address of argv[argc].
	add	r1, r1, r0, asl #2
	mov	r2, r0

	@ Push argv pointers from argv[argc-1] .. argv[0].
copy_loop:
	cmp	r2, $0
	ldrgt	ip, [r1, #-4]!
	strgt	ip, [sp, #-4]!
	subgt	r2, r2, #1
	bgt	copy_loop

	@ Push argc.
	str	r0, [sp, #-4]!

	@ Jump to program.
	ldr	pc, =LOAD_ADDR


	.comm saved_sp, 4, 4

	.bss
	ALIGN8
GLOBAL(irq_stack_lo)
	.space 8
GLOBAL(irq_stack_hi)



