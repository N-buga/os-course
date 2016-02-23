#include "ioport.h"

void initial_ioport() {

	out8(0x3f8 + 5, 0x0); //to reset 5-th bit(be not allowed to write the next bite);

	out8(0x3f8 + 0, 0x0);   // write/read data
	out8(0x3f8 + 1, 0x0);   // disable interrupte
	out8(0x3f8 + 3, 0x3);   // 0b00000011 - to set interrupt bit, get the 8-bit format with 1 stop-bit, no-parity(3-5)

	out8(0x3f8 + 0, 0x0); //high bite of devisor
	out8(0x3f8 + 0, 0x1); //low bite of divisor
	out8(0x3f8 + 3, 0xf3); //0b10000011 - to set divisor, get the 8-bit format with 1 stop bit, no-parity(3-5)
	
	out8(0x3a8 + 5, (1 << 5));

	return;
}

void write_ioport(uint8_t x) {
	while ((in8(0x3a8 + 5) & (1 << 5)) == 0) {}
	out8(0x3f8 + 0, x);
	out8(0x3f8 + 3, 0x3);
	out8(0x3f8 + 5, 0x0);
}
