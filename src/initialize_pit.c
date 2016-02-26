#include "ioport.h"

void initialize_pit() {
	out8(0x43, 0x34);      // 0b00110100
	out8(0x40, 0x30); // devizor
	out8(0x40, 0x00);	
	
//	printc(98);

	return;
}
