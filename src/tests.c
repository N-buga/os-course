#include <sys/types.h>
#include "ioport.h"
#include "threads.h"
#include "locks.h"
#include "io.h"

void schedule();

void* print_a(void* a) {
	(void) a;
	for (int i = 0; i < 12; i++) {
		prints("a ");
		yield();
	}
	prints("end_of_func_a\n");
	return a;
}

void* print_w(void* w) {
	(void) w;
	for (int i = 0; i < 2; i++) {
		prints("w ");
		yield();
	}
	prints("end_of_func_w\n");
	return w;
}

void* print_b(void* b) {
	(void) b;
	for (int i = 0; i < 12; i++) {
		prints("b ");
		yield();
	}
	prints("end_of_func_b");
	return b;
}

void* long_print_t(void* t) {
	(void) t;
	int a = 0;
	while(1 == 1) {
		a += 1;
		if (a % 1000 == 0) prints("t ");
	}
	return t;
}

struct lock_descriptor lock_test;

void* print_a_lock(void* a) {
	lock(&lock_test);
		(void) a;
		for (int i = 0; i < 4; i++) {
			prints("a ");
			yield();	
		}
		prints("end_of_func_a\n");
	unlock(&lock_test);
	return a;			
}

void* print_b_lock(void* b) {
	lock(&lock_test);
		(void) b;
		for (int i = 0; i < 4; i++) {
			prints("b ");
			yield();
		}
		prints("end_of_func_b\n");
	unlock(&lock_test);	
	return b;		
}

void* print_arg(void* a) {
	prints("argument is - ");
	printc(*(char*)a);
	printc('\n');
	return a;
}

void test_simple() {
	pid_t id1 = create_thread(&print_a, (void*) 0);
	pid_t id2 = create_thread(&print_b, (void*) 0);
	(void) id1;
	(void) id2;
}

char c = 'q';

void test_argument() {
	pid_t id1 = create_thread(&print_arg, (void*)&c);
	(void) id1;	
}


void test_simple_lock() {
	pid_t id1 = create_thread(&print_a_lock, (void*) 100);
	pid_t id2 = create_thread(&print_b_lock, (void*) 0);
	
	(void) id1;
	(void) id2;
}

void test_join() {
	pid_t id = create_thread(&print_w, (void*) 0);
	join(id);
	prints("after_join\n");
}

void test_stop() {
	pid_t id = create_thread(&long_print_t, (void*) 0);
	printf("Stop test is thred %d\n", id);
	yield();
	stop(id);
}
