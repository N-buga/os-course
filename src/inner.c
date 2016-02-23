#include "ioport.h"

void inner_serial() {
	prints("Hello!");
	out8(0x20, 0x20);
}


