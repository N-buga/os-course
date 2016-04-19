# os-course

#Task 1
1. src/create_idt.c - creates idt with 32 + 16 raws. Timer interrupt is described by 33th raw(32th if we're counting with null).
2. src/initialize_pic.c - initializes interrupt controller.
3. src/initialize_pit.c - initializes interrupt timer.
4. src/initialize_serial.c - initializes uart. Contains also some functions to write in the port.
5. src/inner.c - inner function-handler for timer interrupt. Is called by outer-function.
6. src/main.c - the main file that calls all initializations and set the idtr.
7. src/other_inner.c - inner function-handler for another interrupts. Is called by other_outer-function.
8. src/other_outer.S - function-handler for another interrupts.
9. src/outer.S - function-handler for timer interrupt.

#Task 2
I use the official solution.

#Task 3
1. src/threads.c - create, finish, join and schedule thread 
2. src/locks.c - realization of locks
3. src/inner.c - to run schedule.
4. src/switcher.S - to change current thread
5. src/start_thread.S - wrapper of function of thread
2. src/tests.c - tests for pthreads
3. src/io.c - function printf

#Task4
1.src/file_system.c - function of file system

2.src/test_file_system.c - tests for function from src/file_system.c
