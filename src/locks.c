#include "locks.h"

void schedule();

void lock(struct lock_descriptor* lock) {
	__asm__("cli");
	while (lock->is_locked == 1) {
		schedule();
	}
	lock->is_locked = 1;
	__asm__("sti");	
}

void unlock(struct lock_descriptor* lock) {
	__asm__("cli");
	lock->is_locked = 0;
	__asm__("sti");
}
