#include<stdint.h>
#include "ioport.h"

extern void printc(uint8_t x);
extern void prints(char* x);
extern void print32(uint32_t x);
extern void print64(uint64_t x);
extern void print64_x(uint64_t x);
extern const uint32_t mboot_info;
extern char text_phys_begin[];
extern char bss_phys_end[];

struct Memory_descriptor {
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
	uint64_t end_addr;
	uint8_t is_eval;
} __attribute__((packed));	


void swap8(uint8_t* a, uint8_t* b) {
	uint8_t x = a[0];
	a[0] = b[0];
	b[0] = x;
}

void swap32(uint32_t* a, uint32_t* b) {
	uint32_t x = a[0];
	a[0] = b[0];
	b[0] = x;
}

void swap64(uint64_t* a, uint64_t* b) {
	uint64_t x = a[0];
	a[0] = b[0];
	b[0] = x;
}

void swap(struct Memory_descriptor* a, struct Memory_descriptor* b) {
	swap32(&(a->size), &(b->size));
	swap64(&(a->base_addr), &(b->base_addr));
	swap64(&(a->length), &(b->length));
	swap64(&(a->end_addr), &(b->end_addr));
	swap8(&(a->is_eval), &(b->is_eval));
	swap32(&(a->type), &(b->type));
}



int insert_kernel() {
	uint64_t begin_kernel = (uint64_t) text_phys_begin;
	uint64_t end_kernel = (uint64_t) bss_phys_end;

	char* ptr = (char*) ((uint64_t) mboot_info);
	ptr += 44;
	uint32_t* ptr_32 = (uint32_t*) ptr;
	uint32_t mmap_length = ptr_32[0];
	uint32_t mmap_begin_addr_value = ptr_32[1];
	uint32_t mmap_addr_value = mmap_begin_addr_value;
	
	struct Memory_descriptor mem_map[50];
	uint32_t count = 0;
	
	while(1) {
		uint32_t* mmap_addr_ptr = (uint32_t*) ((uint64_t) mmap_addr_value);
		uint32_t size = mmap_addr_ptr[0];
		mmap_addr_ptr += 1;
		uint64_t base_addr = ((uint64_t*) mmap_addr_ptr)[0];
		mmap_addr_ptr += 2;
		uint64_t length = ((uint64_t*) mmap_addr_ptr)[0];
		mmap_addr_ptr += 2;
		uint32_t type = ((uint32_t*) mmap_addr_ptr)[0];
		
		mem_map[count].size = size;
		mem_map[count].base_addr = base_addr;
		mem_map[count].length = length;
		mem_map[count].end_addr = length + base_addr;
		mem_map[count].type = type;
		mem_map[count].is_eval = 1;
		
		count++;		

		mmap_addr_value += 4 + size;
		if (mmap_addr_value >= mmap_begin_addr_value + mmap_length)
			break;
	}
	
	uint32_t last_pos = count;
	
	for (uint32_t i = 0; i < count; i++) {
		if (mem_map[i].end_addr <= begin_kernel || mem_map[i].base_addr >= end_kernel) { continue; }
		else {
			mem_map[i].is_eval = 0;
			if (mem_map[i].base_addr < begin_kernel) {
				mem_map[last_pos].length = begin_kernel - mem_map[i].base_addr;
				mem_map[last_pos].end_addr = begin_kernel;
				mem_map[last_pos].is_eval = 1;
				mem_map[last_pos].type = mem_map[i].type;
				mem_map[last_pos++].base_addr = mem_map[i].base_addr;
			}
			if (mem_map[i].end_addr > end_kernel) {
				mem_map[last_pos].length = mem_map[i].end_addr - end_kernel;
				mem_map[last_pos].base_addr = end_kernel;
				mem_map[last_pos].end_addr = mem_map[i].end_addr;
				mem_map[last_pos].type = mem_map[i].type;
				mem_map[last_pos++].is_eval = 1;
			}
		} 
	}
	mem_map[last_pos].length = end_kernel - begin_kernel;
	mem_map[last_pos].base_addr = begin_kernel;
	mem_map[last_pos].end_addr = end_kernel;
	mem_map[last_pos].type = 3;
	mem_map[last_pos++].is_eval = 1;
	
	for (uint32_t i = 0; i < last_pos; i++) {
		for (uint32_t j = i + 1; j < last_pos; j++) {
			if (mem_map[j].base_addr < mem_map[i].base_addr) {
				swap(mem_map + i, mem_map + j);
			}
		}
	}
	
	uint32_t size = sizeof(struct Memory_descriptor) - 4;
	struct Memory_descriptor* ptr_descr = (struct Memory_descriptor*) ((uint64_t) mmap_begin_addr_value);
	for (uint32_t i = 0; i < last_pos; i++) {
		if (mem_map[i].is_eval == 0) continue;
		ptr_descr[0] = mem_map[i];
		ptr_descr[0].size = size;
		ptr_descr++;	
	}	
		
	ptr_32[0] = (uint32_t) ((uint64_t) ptr_descr) - mmap_begin_addr_value;
		
	return 0;
}

void print_mmap() {
	char* ptr = (char*) ((uint64_t) mboot_info);
	ptr += 44;
	uint32_t* ptr_32 = (uint32_t*) ptr;
	uint32_t mmap_length = ptr_32[0];
	uint32_t mmap_begin_addr_value = ptr_32[1];
	uint32_t mmap_addr_value = mmap_begin_addr_value;
	
	while(1) {
		uint32_t* mmap_addr_ptr = (uint32_t*) ((uint64_t) mmap_addr_value);
		uint32_t size = mmap_addr_ptr[0];
		mmap_addr_ptr += 1;
		uint64_t base_addr = ((uint64_t*) mmap_addr_ptr)[0];
		mmap_addr_ptr += 2;
		uint64_t length = ((uint64_t*) mmap_addr_ptr)[0];
		mmap_addr_ptr += 2;
		uint32_t type = ((uint32_t*) mmap_addr_ptr)[0];
		print64(base_addr);
		prints(" - ");
		print64(base_addr + length);
		prints("  --  ");
		if (type == 1) {
			prints("available\n");
		} else if (type == 3) {
			prints("kernel\n");
		}
		else {
			prints("reserved\n");
		}
		mmap_addr_value += 4 + size;
		if (mmap_addr_value >= mmap_begin_addr_value + mmap_length)
			break;
	}
}
