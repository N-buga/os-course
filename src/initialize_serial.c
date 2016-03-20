#include "ioport.h"

void printc(uint8_t x) {
	while (!(in8(0x3f8 + 5) & (1 << 5))) {}
	out8(0x3f8 + 0, x);
}

void prints(char* x) {
	while (!(in8(0x4f8 + 5) & (1 << 5))) {}
	while ((*x) != 0) {
		printc(*x);
		x++;
	} 
}

void print32(uint32_t x) {
	if (x == 0) { printc('0'); return; }
	char number[10];
	int j = 0; 
	while (x > 0) {
		number[j++] = (x % 10) + '0';
		x = x/10;
	}
	for (int i = (j - 1); i >= 0; i--) {
		printc(number[i]);
	}	
}

void print64(uint64_t x) {
	if (x == 0) { printc('0'); return; }
	char number[20];
	int j = 0; 
	while (x > 0) {
		number[j++] = (x % 10) + '0';
		x = x/10;
	}
	for (int i = (j - 1); i >= 0; i--) {
		printc(number[i]);
	} 
}

void print64_x(uint64_t x) {
	if (x == 0) { printc('0'); return; }
	char number[20];
	int j = 0; 
	while (x > 0) {
		if (x%16 <= 9) {
			number[j++] = (x % 16) + '0'; 
		} else {
			number[j++] = 'a' + (x % 16) - 10;
		}
		x = x/16;
	}
	for (int i = (j - 1); i >= 0; i--) {
		printc(number[i]);
	} 
}

void initialize_serial() {

	out8(0x3f8 + 5, 0x0); //to reset 5-th bit(be not allowed to write the next bite);

	out8(0x3f8 + 3, 0x3);   // 0b00000011 - to set interrupt bit, get the 8-bit format with 1 stop-bit, no-parity(3-5)
	out8(0x3f8 + 1, 0x0);   // disable interrupte

	out8(0x3f8 + 3, 0x83); //0b10000011 - to set divisor, get the 8-bit format with 1 stop bit, no-parity(3-5)
	out8(0x3f8 + 0, 0x10); // low bite of devisor
	out8(0x3f8 + 1, 0x0); //high bite of divisor

	out8(0x3f8 + 3, 0x3);	// to switch the write/read mode
	out8(0x3a8 + 5, (1 << 5)); //be allowed to write next bite

//	printc(0x61);

	return;
}
