#include <sys/types.h>
#include "threads.h"
#include "kmem_cache.h"
#include "memory.h"
#include "ioport.h"
#include "io.h"

pid_t head_running = 0;
pid_t head_free = 1;
int cur_thread = 0;
struct thread_pool thread_pool;

void schedule();

struct Init_data {
	uint64_t r15, r14, r13, r12, rbx, rbp;
        void* start_thread_addr;
        void* fun_addr;
        void* arg;	
};
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

void init_thread_pool() {
	for (int i = 0; i < MAX_THREAD_NUMBER; i++) {
		thread_pool.threads[i].state = NOT_STARTING;
	}
	thread_pool.threads[0].state = RUNNING;
	thread_pool.next_running_thread[0] = 0;
	thread_pool.prev_running_thread[0] = 0;
	for (int i = 1; i < MAX_THREAD_NUMBER - 1; i++) {
		thread_pool.next_free_thread[i] = i + 1;
	}
	thread_pool.next_free_thread[MAX_THREAD_NUMBER - 1] = 1;
	for (int i = 2; i < MAX_THREAD_NUMBER; i++) {
		thread_pool.prev_free_thread[i] = i - 1;
	} 
	thread_pool.prev_free_thread[1] = MAX_THREAD_NUMBER - 1;
	
}

struct thread_state* get_next_schedule_thread() {
	pid_t result = thread_pool.next_running_thread[cur_thread];
	cur_thread = result;
	return &(thread_pool.threads[result]);
}

pid_t create_thread(void* (*fptr)(void *), void *arg) {
	__asm__("cli");
		
	pid_t new_thread = head_free;
	head_free = thread_pool.next_free_thread[head_free];
	thread_pool.next_free_thread[thread_pool.prev_free_thread[new_thread]] = thread_pool.next_free_thread[new_thread];
	thread_pool.prev_free_thread[thread_pool.next_free_thread[new_thread]] = thread_pool.prev_free_thread[new_thread];
	thread_pool.threads[new_thread].start_stack = kmem_alloc(2*PAGE_SIZE);
	thread_pool.threads[new_thread].ptr_stack = thread_pool.threads[new_thread].start_stack + 2*PAGE_SIZE - sizeof(struct Init_data);
		
	struct Init_data* ptr_data = thread_pool.threads[new_thread].ptr_stack;
	ptr_data->r12 = 0;
	ptr_data->r13 = 0;
	ptr_data->r14 = 0;
	ptr_data->r15 = 0;
	ptr_data->rbx = 0;
	ptr_data->rbp = 0;
	
	extern void *start_thread;
    	ptr_data->start_thread_addr = &start_thread;

   	ptr_data->fun_addr = fptr;
     	ptr_data->arg = arg;
     	
    	thread_pool.threads[new_thread].state = RUNNING;
    	
    	int next_cur_thread = thread_pool.next_running_thread[cur_thread];
    	thread_pool.next_running_thread[cur_thread] = new_thread;
    	thread_pool.prev_running_thread[next_cur_thread] = new_thread;
    	thread_pool.prev_running_thread[new_thread] = cur_thread;
    	thread_pool.next_running_thread[new_thread] = next_cur_thread;
    	
    	__asm__("sti");
    	return new_thread;				
}

void join(pid_t id_thread) {
	while (thread_pool.threads[id_thread].state != STOP) { 
		__asm__("cli");
		schedule();
		__asm__("sti");
	}
	kmem_free(thread_pool.threads[id_thread].start_stack);
}

void exit_thread() {
	__asm__("cli");
	thread_pool.threads[cur_thread].state = STOP;
	thread_pool.next_running_thread[thread_pool.prev_running_thread[cur_thread]] = thread_pool.next_running_thread[cur_thread];
	thread_pool.prev_running_thread[thread_pool.next_running_thread[cur_thread]] = thread_pool.prev_running_thread[cur_thread];
	schedule();
	__asm__("sti");	
}

