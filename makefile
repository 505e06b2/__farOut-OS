#https://launchpad.net/~tkchia/+archive/ubuntu/build-ia16/
#https://github.com/tkchia/gcc-ia16/wiki

FLOPPY=floppy.img
BOOT=obj/bootloader.bin
START=obj/start.o

KERNEL_OUT=floppy_contents/kernel.com
SHELL_OUT=floppy_contents/shell.com
STDLIB_OUT=floppy_contents/libc.com

STANDARD_LIB_OBJS := $(patsubst src/standard_library/%.c,obj/lib_%.o,$(wildcard src/standard_library/*.c))
KERNEL_OBJS := obj/kernel_main.o $(filter-out obj/kernel_main.o,$(patsubst src/kernel/%.c,obj/kernel_%.o,$(wildcard src/kernel/*.c)))
SHELL_OBJS := $(patsubst src/shell/%.c,obj/shell_%.o,$(wildcard src/shell/*.c))
#must force main.c to be the first linked

AS=nasm -f bin
LD=ia16-elf-ld
CC=ia16-elf-gcc

#Optimising
#gc-sections doesnt work and may not be very good when freestanding, it also seems like this compiler doesn't have LTO
CFLAGS=-ffreestanding -Wall -march=i8086 -mtune=i8086 -masm=intel -mcmodel=tiny -std=gnu99 -Os -MMD -MP
LDFLAGS=--oformat=binary -m i386msdos

.PHONY: run init clean debug_kernel fake86

all: $(BOOT) $(STDLIB_OUT) $(KERNEL_OUT) $(SHELL_OUT)

$(BOOT): src/bootloader.nasm
	$(AS) $^ -o $@

$(START): src/start.c
	$(CC) $(CFLAGS) -c $< -o $@

$(STDLIB_OUT): $(STANDARD_LIB_OBJS)
	$(LD) $(LDFLAGS) -Map=stdlib_symbols.map $^ -o $@
	./generate_farptrs.py stdlib

$(KERNEL_OUT): $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -Map=kernel_symbols.map $^ -o $@
	./generate_farptrs.py kernel

$(SHELL_OUT): $(START) $(SHELL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

obj/kernel_%.o: src/kernel/%.c
	$(CC) $(CFLAGS) -Isrc/ -Isrc/kernel/include/ -c $< -o $@

obj/shell_%.o: src/shell/%.c $(STDLIB_OUT) $(KERNEL_OUT)
	$(CC) $(CFLAGS) -Isrc/ -Isrc/generated_stdlib_includes/ -Isrc/generated_kernel_includes/ -c $< -o $@

obj/lib_%.o: src/standard_library/%.c $(KERNEL_OUT)
	$(CC) $(CFLAGS) -Isrc/standard_library/include/ -Isrc/generated_kernel_includes/ -c $< -o $@

#depenancies
-include $(KERNEL_OBJS:%.o=%.d)
-include $(SHELL_OBJS:%.o=%.d)
-include $(STANDARD_LIB_OBJS:%.o=%.d)

init:
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	mkfs.fat -D 0 -n "5OS" $(FLOPPY)

run: all
	dd conv=notrunc if=$(BOOT) of=$(FLOPPY) bs=1 skip=62 seek=62
	sudo mount -o loop $(FLOPPY) /mnt/
	sudo cp floppy_contents/* /mnt/
	sudo umount /mnt/
	qemu-system-i386 -monitor stdio -m 1M -drive format=raw,index=0,if=floppy,file=$(FLOPPY)

fake86: all
	dd conv=notrunc if=$(BOOT) of=$(FLOPPY) bs=1 skip=62 seek=62
	sudo mount -o loop $(FLOPPY) /mnt/
	sudo cp floppy_contents/* /mnt/
	sudo umount /mnt/
	fake86 -fd0 $(FLOPPY)

debug_kernel: $(KERNEL_OUT)
	ndisasm -b 16 -p intel $<

files:
	/bin/ls -la floppy_contents/

clean:
	#rm -f $(FLOPPY)
	rm -f obj/*
	rm -f floppy_contents/*.com
	rm -f src/generated_stdlib_includes/*
	rm -f src/generated_kernel_includes/*
