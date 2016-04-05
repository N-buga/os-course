#include "ioport.h"
#include "threads.h"
#include "io.h"

int jeffies = 0;
extern int cur_thread;
extern struct thread_pool thread_pool;
extern int count_pthreads;

int switch_threads(void **old_sp, void *new_sp);
struct thread_state* get_next_schedule_thread();

void schedule() {
	int old_thread = cur_thread;
	struct thread_state* next_state = get_next_schedule_thread();
	printf("cur_thread %d \n", cur_thread);
	printf("%p\n", next_state->ptr_stack);
	if (next_state != thread_pool.threads + old_thread) {
		switch_threads(&(thread_pool.threads[old_thread].ptr_stack), next_state->ptr_stack);
	}
}

const int MAX_TIME = (int)1000;

void inner_serial() {
	//__asm__("cli");
	out8(0x20, 0x20);
//	prints("$");
	if (jeffies < MAX_TIME) {
//		prints("Hello!");
//		printf("%d %d\n", cur_thread, jeffies);
	} else {
		prints("---------\n");
		printf("%d %d\n", cur_thread, jeffies);
		jeffies = 0;
		schedule();
	}
	jeffies++;
	//__asm__("sti");
}


