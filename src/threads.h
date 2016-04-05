#pragma once

#include <sys/types.h>

#define MAX_THREAD_NUMBER 1000 

typedef enum {RUNNING, STOP, NOT_STARTING} EVAL_STATE;

struct thread_state {
	void* start_stack;
	void* ptr_stack;
	EVAL_STATE state;		
};

struct thread_pool {
	pid_t first_free;
	struct thread_state threads[MAX_THREAD_NUMBER];
	pid_t next_running_thread[MAX_THREAD_NUMBER];
	pid_t prev_running_thread[MAX_THREAD_NUMBER];
	pid_t next_free_thread[MAX_THREAD_NUMBER];
	pid_t prev_free_thread[MAX_THREAD_NUMBER];
};

pid_t create_thread(void* (*fptr)(void *), void *arg);
void init_thread_pool();
void join(pid_t id_thread);
void schedule();
