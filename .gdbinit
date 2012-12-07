set architecture armv5te
target remote localhost:1234
add-symbol-file ./kernel/kernel 0xa3000000
add-symbol-file ./tasks/bin/dagger 0xa0000000
b app_startup
b time_init
b mutex_init
b toUser
b *0xa0000000
b task_create
b disable_interrupts
b runqueue_init
b dev_init
b allocate_tasks
b runqueue_add
b dispatch_init
b dispatch_nosave
b dispatch_save
b dev_update
b *0xa3000000
b *0xa3000008
b *0xa300004c
b *0xa30000a0
b *0xa30000cc
b *0xa3000dac
b *0xa3000ddc
b *0xa3000de8
b *0xa0000170
b *0xa000018c
