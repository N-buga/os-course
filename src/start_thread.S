.global start_thread

start_thread:
	sti

	popq %rax         // get thread function address
	popq %rdi         // get thread function args

	callq *%rax       // call real thread function

	callq exit_thread
	loop: jmp loop
