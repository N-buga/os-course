.extern other_inner
.global other_outer

other_outer:
	push %rax
	push %rcx
	push %rdx
	push %rbx
	push %rsp
	push %rbp
	push %rsi
	push %rdi
	mov %rbp, %rsp
	call other_inner
	pop %rdi
	pop %rsi
	pop %rbp
	pop %rsp
	pop %rbx
	pop %rdx
	pop %rcx
	pop %rax
	iretq
