#include "ioport.h"
#include "threads.h"
#include "io.h"

int jeffies = 0;
extern int cur_thread;
extern struct thread_pool thread_pool;
extern int count_pthreads;

const int MAX_TIME = (int)1000;

void inner_serial() {
	//__asm__("cli");
	out8(0x20, 0x20);
//	prints("$");
	if (jeffies < MAX_TIME) {
//		prints("Hello!");
//		printf("%d %d\n", cur_thread, jeffies);
	} else {
//		prints("---------\n");
//		printf("%d %d\n", cur_thread, jeffies);
		jeffies = 0;
		schedule();
	}
	jeffies++;
	//__asm__("sti");
}


