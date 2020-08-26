#https://launchpad.net/~tkchia/+archive/ubuntu/build-ia16/
#https://github.com/tkchia/gcc-ia16/wiki

BOOT=obj/bootloader.bin
LIBS := $(patsubst src/libs/%.c,obj/lib_%.o,$(wildcard src/libs/*.c))
KERNEL=floppy_contents/kernel.com
FLOPPY=floppy.img

AS=nasm -f bin
LD=ia16-elf-ld
CC=ia16-elf-gcc

CFLAGS=-c -Isrc/libs/include/ -ffreestanding -Wall -march=i8086 -mtune=i8086 -masm=intel -mcmodel=tiny -std=gnu99 -O0
LDFLAGS=--oformat=binary -mi386msdos

.PHONY: run init clean debug_kernel

all: $(BOOT) $(KERNEL)

$(BOOT): src/bootloader.nasm
	$(AS) $^ -o $@

$(KERNEL): obj/kernel.o $(LIBS)
	$(LD) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

# Must use a header file for each c file - makes sure that if the header is changed in any lib, the lib is recompiled
obj/lib_%.o: src/libs/%.c src/libs/include/%.h
	$(CC) $(CFLAGS) $< -o $@

init:
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	mkfs.fat -F 12 -D 0 -n "5OS" $(FLOPPY)

run: all
	dd conv=notrunc if=$(BOOT) of=$(FLOPPY) bs=1 skip=62 seek=62
	sudo mount -o loop $(FLOPPY) /mnt/
	sudo cp floppy_contents/* /mnt/
	sudo umount /mnt/
	qemu-system-i386 -drive file=$(FLOPPY),index=0,media=disk,format=raw

debug_kernel: $(KERNEL)
	ndisasm -b 16 -p intel $<

clean:
	rm -f $(FLOPPY)
	rm -f obj/*
	rm -f contents/*
