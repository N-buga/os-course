.extern other_inner_slave
.global other_outer_slave

other_outer_slave:
	cli
	push %rax
	push %rcx
	push %rdx
	push %rbx
	push %rsp
	push %rbp
	push %rsi
	push %rdi
	call other_inner_slave
	pop %rdi
	pop %rsi
	pop %rbp
	pop %rsp
	pop %rbx
	pop %rdx
	pop %rcx
	pop %rax
	sti
	iretq
