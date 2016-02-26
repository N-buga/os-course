#include "interrupt.h"
#include "ioport.h"

void initialize_pic() {
	
	out8(0x20, 0x11);  // 0b0001 0001
	out8(0xA0, 0x11); // 0b0001 0001

	out8(0x21, 0x20);
	out8(0xA1, 0x28);

	out8(0x21, 0x04); // 0b0000 0100
	out8(0xA1, 0x02); // 0b0000 0010

	out8(0x21, 0x01); // 0b0000 0001
	out8(0xA1, 0x01); // 0b00000001

//	printc(99);
	
	return;
}
