.extern hardware_inner
.global hardware_outer

hardware_outer:
	cli
	push %rax
	push %rcx
	push %rdx
	push %rbx
	push %rsp
	push %rbp
	push %rsi
	push %rdi
	call hardware_inner
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
