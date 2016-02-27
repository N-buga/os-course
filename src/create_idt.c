#include "interrupt.h"
#include "memory.h"
#include "ioport.h"

void outer_serial();
void other_outer_master();
void other_outer_slave();
void hardware_outer();

struct Descriptor ptr[32 + 16];

void set_descriptor(int i, uint64_t address) {
	ptr[i].reserved = 0;
	ptr[i].offset_high = (address >> 32);
	ptr[i].offset_mid = (address >> 16) & 0xffff;
	ptr[i].offset_low = address & 0xffff;
	ptr[i].segment_selector = KERNEL_CODE;
	ptr[i].config = 0xee00;   //0b1110 1110 0000 0000;	
}

void create_idt() {	

//	printc(100);
	uint64_t timer_handler = (uint64_t) &outer_serial;
	uint64_t other_handler_master = (uint64_t) &other_outer_master;
	uint64_t other_handler_slave = (uint64_t) &other_outer_slave;
	uint64_t hardware_handler = (uint64_t) &hardware_outer;
	set_descriptor(32, timer_handler); 
	for (int i = 1; i < 8; i++) {
		set_descriptor(32 + i, other_handler_master);
	}
	for (int i = 8; i < 16; i++) {
		set_descriptor(32 + i, other_handler_slave);
	}
	for (int j = 0; j < 32; j++) {
		set_descriptor(j, hardware_handler);
	} 
	
	return;		
}
