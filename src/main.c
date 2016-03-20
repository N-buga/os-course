#include "interrupt.h"
#include "ioport.h"

extern struct Descriptor ptr[32 + 16];
extern void insert_kernel();
extern void print_mmap();

struct idt_ptr idtr;

void main(void)
{ 
/*	__asm__("cli");
	initialize_serial();
	initialize_pit();
	initialize_pic();
	create_idt();
	
	idtr.base = (uint64_t)ptr;
	idtr.size = sizeof(ptr) - 1;;

	set_idt(&idtr);	

	__asm__("sti");
*/
	initialize_serial();
	insert_kernel();
	print_mmap();
	
	while (1) {}
}
