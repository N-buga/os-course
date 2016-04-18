#include "interrupt.h"
//#include "interrupt_.h"
#include "ioport.h"
#include "locks.h"
#include "paging.h"
#include "memory.h"
#include "misc.h"
#include "threads.h"
#include "kmem_cache.h"
#include "tests.h"
#include "test_file_system.h"

extern struct Descriptor ptr[32 + 16];

struct idt_ptr idtr;
struct lock_descriptor global_lock;
extern int jeffies;

void inner_serial();
void initialize_serial();
void initialize_pit();
void initialize_pic();


void main(void)
{ 
	__asm__("cli");
	global_lock.is_locked = 0;
	
	initialize_serial();
	initialize_pit();
	initialize_pic();

	create_idt();

	init_thread_pool();
	
	setup_misc();
	setup_memory();
	setup_buddy();
	setup_paging();
	setup_alloc();
	
	idtr.base = (uint64_t)ptr;
	idtr.size = sizeof(ptr) - 1;;

	set_idt(&idtr);	
	
	__asm__("sti");

//	test_simple();	
//	test_simple_lock();
//	test_join();
//	test_argument();
//	test_stop();
	prints("--------------\n");
	testRoot();
	prints("----------\n");
	testCreate();
	prints("----------\n");
	testReadWrite();
	prints("-----------\n");
	testFileByPath();
	
	while (1) {}
}

void debug() {
	prints("here");
}
