#include "ioport.h"

void other_inner() {
	prints("Bue!");
	out8(0x20, 0x20);
}

