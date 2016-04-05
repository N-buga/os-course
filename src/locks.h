#pragma once

#include<stdint.h>

struct lock_descriptor {
	uint8_t is_locked;
};

void lock(struct lock_descriptor* lock);
void unlock(struct lock_descriptor* lock);
