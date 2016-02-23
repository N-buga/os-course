#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>

void switch_off();
void initialize_serial();
void initialize_pit();
void initialize_pic();
void create_idt();

struct idt_ptr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

static inline void set_idt(const struct idt_ptr *ptr)
{ __asm__ volatile ("lidt (%0)" : : "a"(ptr)); }

struct Descriptor {
	uint16_t offset_low;	 
	uint16_t segment_selector;
	uint16_t config;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t reserved;
} __attribute__((packed));

#endif /*__INTERRUPT_H__*/
