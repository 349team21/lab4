set architecture armv5te
target remote localhost:1234
add-symbol-file ./kernel/kernel 0xa3000000
add-symbol-file ./tasks/bin/dagger 0xa0000000
b main.c:46
b time.c:41
b main.c:115
b mutex.c:30
b main.c:119
b *0xa0000000
b dagger.c:53
b proc.c:25
b run_queue.c:15
b device.c:46
b sched.c:57
b ctx_switch.c:30
b ctx_switch.c:78
b run_queue.c:86
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
