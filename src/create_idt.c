#include "interrupt.h"
#include "memory.h"
#include "ioport.h"

void outer_serial();

struct Descriptor ptr[32 + 16];

void create_idt() {	

	printc(100);
	uint64_t serial_long = (uint64_t) &outer_serial;
	ptr[32].reserved = 0;
	ptr[32].offset_high = (serial_long >> 32);
	ptr[32].offset_mid = (serial_long >> 16) & 0xffff;
	ptr[32].offset_low = serial_long & 0xffff;
	ptr[32].segment_selector = KERNEL_CODE;
	ptr[32].config = 0xee00;   //0b1110 1110 0000 0000;
	
	return;		
}
