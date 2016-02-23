#include "interrupt.h"
#include "ioport.h"

void initial_interrupt() {
	out8(0x20, 0b00010001);
	out8(0xA0, 0b00010001);

	out8(0x21, 0x20);
	out8(0xA1, 0x28);

	out8(0x21 + 1, 0b00000100);
	out8(0xA1 + 1, 0b00000010);

	out8(0x21 + 2, 0b00000001);
	out8(0xA1 + 2, 0b00000001);
	
	return;
}
