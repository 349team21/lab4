set architecture armv5te
target remote localhost:1234
add-symbol-file ./kernel/kernel 0xa3000000
add-symbol-file ./tasks/bin/dagger 0xa0000000
b main.c:43
b main.c:46
b main.c:111
b time.c:37
b main.c:115
b mutex.c:30
b main.c:119
b start.S:135
b *0xa0000000
b dagger.c:53
b proc.c:25
b run_queue.c:15
b device.c:46
b sched.c:57
b ctx_switch.c:30
b ctx_switch.c:78
b run_queue.c:86
b ctx_switch_asm.S:54


