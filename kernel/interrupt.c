
void interrupt(void) {
	printf("INTERRUPTED!\n");
	disable_interrupts();
	current_time = reg_read(OSTMR_OSCR_ADDR);
	unsigned int actual_time = current_time/OSTMR_FREQ;
	dev_update(actual_time);
	reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
	reg_write(OSTMR_OSMR_ADDR(0), current_time + (OSTMR_FREQ/100));

	dispatch_save();
}	
