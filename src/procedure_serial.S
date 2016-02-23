.global procedure_serial

.extern write_ioport

procedure_serial:
	pushq %rbp
	mov %rbp, %rsp
	push 97
	call write_ioport  // call function
	add %rsp, 12   // clear the stack
	popq %rbp   // recover current frame register 

	pushq %rbp  // remember current frame register
	mov %rbp, %rsp   // do the beginning of new frame register as the current pointer of stack
	push 0x20     // argument 2
	push 0x20   // argument 1
	call write_ioport  // call function
	add %rsp, 12   // clear the stack
	popq %rbp   // recover current frame register 

	iretq
