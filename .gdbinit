set architecture armv5te
target remote localhost:1234
add-symbol-file ./kernel/kernel 0xa3000000
add-symbol-file ./tasks/bin/dagger 0xa0000000
b kmain
b time_init
b mutex_init
b proc.c:28
b runqueue_init
b dev_init
b allocate_tasks
b runqueue_add
b dispatch_init
b dispatch_nosave
b dispatch_save
b dev_update
b assign_schedule
b *(_start)
b *(handleSWI)
b *(handleIRQ)
b *(copy_loop)
b *(launch_task)
b *(ctx_switch_half)
b *(loop)
b *(write)
b *(success)

