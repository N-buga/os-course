.extern other_inner_master
.global other_outer_master

other_outer_master:
	cli
	push %rax
	push %rcx
	push %rdx
	push %rbx
	push %rsp
	push %rbp
	push %rsi
	push %rdi
	call other_inner_master
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
