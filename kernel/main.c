/** @file main.c
 *
 * @brief kernel main
 *
 * @author 
 *	   
 *	   
 * @date   
 */
 
#include <kernel.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <assert.h>

#define SWI_VECTOR_ADDR 0x00000008
#define IRQ_VECTOR_ADDR 0x00000018
#define PC_OFFSET       8
#define LDR_PC_BASE		0xe51ff000 /* ldr pc, [pc, #imm12] */
#define LDR_U_FLAG   	0x00800000
#define LDR_IMM_MASK 	0x00000fff
#define LDR_PC_NEXT  	0xe51ff004 /* ldr pc, [pc, #-4] */

extern void handleSWI(void);
extern void handleIRQ(void);
volatile size_t current_time;
volatile size_t start_time;

uint32_t global_data;

int kmain(int argc __attribute__((unused)), char** argv  __attribute__((unused)), uint32_t table)
{

	app_startup();
	global_data = table;
	/* add your code up to assert statement */
	
	unsigned long* old_swi_handler;
	unsigned long* old_irq_handler;
	
	unsigned long swi_vector = *(unsigned long *)SWI_VECTOR_ADDR;
	unsigned long irq_vector = *(unsigned long *)IRQ_VECTOR_ADDR;
	long swi_offset;
	long irq_offset;
	
	
	
	// install_swi_handler
	/* Check if the SWI/IRQ vector addr contains a "ldr pc, [pc, #imm12] instruction. */
	if ((swi_vector & ~(LDR_U_FLAG | LDR_IMM_MASK)) != LDR_PC_BASE)
	{
		printf("Kernel panic: unrecognized vector: %08lx\n", swi_vector);
		return 0x0badc0de;
	}

	/* Determine the literal offset. */
	if (swi_vector & LDR_U_FLAG) {
		swi_offset = swi_vector & LDR_IMM_MASK;
	}
	else {
		swi_offset = -(swi_vector & LDR_IMM_MASK);
	}

	/* Find the location of U-Boot's SWI/IRQ handler. */
	old_swi_handler = *(unsigned long **)(SWI_VECTOR_ADDR + PC_OFFSET + swi_offset);

	/* Redirect U-Boot's SWI handler to ours. */
	*(old_swi_handler  ) = LDR_PC_NEXT;
	*(old_swi_handler + 1) = (unsigned long) &handleSWI;
	
	
	
	
	// install_irq_handler
	/* Check if the SWI/IRQ vector addr contains a "ldr pc, [pc, #imm12] instruction. */
	if ((irq_vector & ~(LDR_U_FLAG | LDR_IMM_MASK)) != LDR_PC_BASE)
	{
		printf("Kernel panic: unrecognized vector: %08lx\n", irq_vector);
		return 0x0badc0de;
	}

	/* Determine the literal offset. */
	if (irq_vector & LDR_U_FLAG) {
		irq_offset = irq_vector & LDR_IMM_MASK;
	}
	else {
		irq_offset = -(irq_vector & LDR_IMM_MASK);
	}

	/* Find the location of U-Boot's SWI/IRQ handler. */
	old_irq_handler = *(unsigned long **)(IRQ_VECTOR_ADDR + PC_OFFSET + irq_offset);

	/* Redirect U-Boot's IRQ handler to ours. */
	*(old_irq_handler  ) = LDR_PC_NEXT;
	*(old_irq_handler+1) = (unsigned long) &handleIRQ;


	//Interrupt setup
	reg_write(INT_ICMR_ADDR, (0x1 << INT_OSTMR_0));
	reg_write(INT_ICLR_ADDR, (0x0 << INT_OSTMR_0));

	//Oscillator setup
	reg_clear(OSTMR_OIER_ADDR, OSTMR_OIER_E1 | OSTMR_OIER_E2 | OSTMR_OIER_E3);
	reg_write(OSTMR_OIER_ADDR, OSTMR_OIER_E0);

	reg_write(OSTMR_OSCR_ADDR, 0x0);
	start_time = reg_read(OSTMR_OSCR_ADDR);
	reg_write(OSTMR_OSMR_ADDR(0), (OSTMR_FREQ/100));
	

	// Initialize mutexes
	mutex_init();

	printf("MAKING USER SWITCH\n");
	toUSER(argc, argv);
	
	assert(0);        /* should never get here */
	printf("How do you get here?!!!\n");
	return 42;
}




