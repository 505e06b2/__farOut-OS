#https://launchpad.net/~tkchia/+archive/ubuntu/build-ia16/
#https://github.com/tkchia/gcc-ia16/wiki

FLOPPY=floppy.img
BOOT=obj/bootloader.bin
KERNEL_OUT=floppy_contents/kernel.com

STANDARD_LIB_OBJS := $(patsubst src/standard_library/%.c,obj/lib_%.o,$(wildcard src/standard_library/*.c))
KERNEL_OBJS := obj/kernel_main.o $(filter-out obj/kernel_main.o,$(patsubst src/kernel/%.c,obj/kernel_%.o,$(wildcard src/kernel/*.c)))
#must force main.c to be the first linked

AS=nasm -f bin
LD=ia16-elf-ld
CC=ia16-elf-gcc

#Optimising
#gc-sections doesnt work and may not be very good when freestanding, it also seems like this compiler doesn't have LTO
CFLAGS=-Isrc/standard_library/include/ -Isrc/kernel/include/ -ffreestanding -Wall -march=i8086 -mtune=i8086 -masm=intel -mcmodel=tiny -std=gnu99 -Os -MMD -MP
LDFLAGS=--oformat=binary -m i386msdos -Map symbols_map.txt

.PHONY: run init clean debug_kernel

all: $(BOOT) $(KERNEL_OUT)

$(BOOT): src/bootloader.nasm
	$(AS) $^ -o $@

$(KERNEL_OUT): $(KERNEL_OBJS) $(STANDARD_LIB_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

obj/kernel_%.o: src/kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/lib_%.o: src/standard_library/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#depenancies
-include $(KERNEL_OBJS:%.o=%.d)
-include $(STANDARD_LIB_OBJS:%.o=%.d)

init:
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	mkfs.fat -D 0 -n "5OS" $(FLOPPY)

run: all
	dd conv=notrunc if=$(BOOT) of=$(FLOPPY) bs=1 skip=62 seek=62
	sudo mount -o loop $(FLOPPY) /mnt/
	sudo cp floppy_contents/* /mnt/
	sudo umount /mnt/
	qemu-system-i386 -fda $(FLOPPY)

debug_kernel: $(KERNEL_OUT)
	ndisasm -b 16 -p intel $<

files:
	/bin/ls -la floppy_contents/

clean:
	#rm -f $(FLOPPY)
	rm -f obj/*
	rm -f $(KERNEL_OUT)
