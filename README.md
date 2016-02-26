# os-course
mit.spbau.ru OS course materials

1. create_idt.c - creates idt with 32 + 16 raws. Timer interrupt is described by 33th raw(32th if we're counting with null).
2. initialize_pic.c - initializes interrupt controller.
3. initialize_pit.c - initializes interrupt timer.
4. initialize_serial.c - initializes uart. Contains also some functions to write in the port.
5. inner.c - inner function-handler for timer interrupt. Is called by outer-function.
6. main.c - the main file that calls all initializations and set the idtr.
7. other_inner.c - inner function-handler for another interrupts. Is called by other_outer-function.
8. other_outer.S - function-handler for another interrupts.
9. outer.S - function-handler for timer interrupt.
