#include "interrupt.h"
#include "ioport.h"

extern struct Descriptor ptr[32 + 16];

struct idt_ptr idtr;

void main(void)
{ 
	prints("In main\n");
	__asm__("cli");
	initialize_serial();
	initialize_pit();
	initialize_pic();
	create_idt();

	idtr.base = (uint64_t)ptr;
	idtr.size = sizeof(struct idt_ptr) * 33 - 1;;

	set_idt(&idtr);	
	__asm__("sti");

	while (1) {}
}
