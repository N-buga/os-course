CC ?= gcc
LD ?= gcc

CFLAGS := -g -m64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -ffreestanding \
	-mcmodel=kernel -Wall -Wextra -Werror -std=c11 \
	-Wframe-larger-than=4096 -Wstack-usage=4096 -Wno-unknown-warning-option \
	-Wno-unused-function
LFLAGS := -nostdlib -z max-page-size=0x1000

ASM := bootstrap.S start_thread.S switcher.S videomem.S outer.S other_outer_slave.S other_outer_master.S hardware_outer.S 
AOBJ:= $(ASM:.S=.o)
ADEP:= $(ASM:.S=.d)

SRC := main.c test_file_system.c file_system.c ctype.c console.c io.c balloc.c list.c error.c string.c \
 initialize_pit.c misc.c threads.c tests.c locks.c kmem_cache.c memory.c \
 paging.c inner.c other_inner_master.c other_inner_slave.c hardware_inner.c \
 initialize_serial.c initialize_pic.c create_idt.c 
OBJ := $(AOBJ) $(SRC:.c=.o)
DEP := $(ADEP) $(SRC:.c=.d)

all: kernel

kernel: $(OBJ) kernel.ld
	$(LD) $(LFLAGS) -T kernel.ld -o $@ $(OBJ)

%.o: %.S
	$(CC) -m64 -D__ASM_FILE__ -g -MMD -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -f kernel $(OBJ) $(DEP)
