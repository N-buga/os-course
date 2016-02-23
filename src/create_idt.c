#include "interrupt.h"
#include "memory.h"

struct Descriptor ptr[32 + 16];

extern void procedure_serial();

void create_idt() {	
	uint64_t proc_long = (uint64_t) &procedure_serial;
	ptr[32].reserved = 0;
	ptr[32].offset_high = (proc_long >> 32);
	ptr[32].offset_mid = (proc_long >> 16) % (1 << 16);
	ptr[32].offset_low = proc_long % (1 << 16);
	ptr[32].segmentSelector = KERNEL_CODE;
	ptr[32].config = 0b1110111000000000;
	
	return;		
}
