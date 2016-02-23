#include "ioport.h"

void initial_timer() {
	out8(0x43, 0x34);      // 0b00110100
	out16(0x40, 0x0030); // devizor	

	return;
}
