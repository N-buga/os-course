#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>

struct idt_ptr {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

static inline void set_idt(const struct idt_ptr *ptr)
{ __asm__ volatile ("lidt (%0)" : : "a"(ptr)); }

struct Descriptor {
	uint32_t reserved;
	uint32_t offset_high;
	uint16_t offset_mid;
	uint16_t config;
	uint16_t segmentSelector;
	uint16_t offset_low;	 
} __attribute__((packed));

#endif /*__INTERRUPT_H__*/
