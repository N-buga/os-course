#include "interrupt.h"

void switch_off();
void initial_ioport();
void initial_timer();
void initial_interrupt();
void creat_idt();

extern struct Descriptor ptr[32 + 16];

struct idt_ptr idtr;

void main(void)
{ 
	switch_off();	
	while (1); 
	initial_ioport();
	initial_timer();
	initial_interrupt();
	create_idt();

	idtr.base = (uint64_t)&ptr;
	idtr.size = 32;

	set_idt(&idtr);	
}
