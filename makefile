#https://launchpad.net/~tkchia/+archive/ubuntu/build-ia16/
#https://github.com/tkchia/gcc-ia16/wiki

BOOT=obj/bootloader.bin
LIBS := $(patsubst src/libs/%.c,obj/lib_%.o,$(wildcard src/libs/*.c))
KERNEL=floppy_contents/kernel.com
FLOPPY=floppy.img

AS=nasm -f bin
LD=ia16-elf-ld
CC=ia16-elf-gcc

#Optimising - Os doesnt work for fs.o - It does some weird stuff with far pointers and memcpy (might try to do some "built-in" optimisations for memcpy)
#gc-sections doesnt work and may not be very good when freestanding, it also seems like this compiler doesn't have LTO - all the more reason to have a separate libc
#-Os 1385 with no memcpy
#-O1 1575 no memcpy
#-O1 -fcrossjumping 1512 no memcpy

#-O1 1588
#-O1 -fcrossjumping -fpeephole2 1518
#-O1 -fcrossjumping -fpeephole2 -fexpensive-optimizations 1481

CFLAGS=-Isrc/libs/include/ -ffreestanding -Wall -march=i8086 -mtune=i8086 -masm=intel -mcmodel=tiny -std=gnu99 -O1 -MMD -MP\
-fcrossjumping\
-fpeephole2\
-fexpensive-optimizations

LDFLAGS=--oformat=binary -mi386msdos -Map symbols_map.txt

.PHONY: run init clean debug_kernel check_files

all: $(BOOT) $(KERNEL)

$(BOOT): src/bootloader.nasm
	$(AS) $^ -o $@

$(KERNEL): obj/kernel.o $(LIBS)
	$(LD) $(LDFLAGS) $^ -o $@

#This will be used for entrypoint applications
obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

obj/lib_%.o: src/libs/%.c
	$(CC) -c $(CFLAGS) $< -o $@

#depenancies
-include obj/kernel.d
-include $(LIBS:%.o=%.d)

init:
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	mkfs.fat -D 0 -n "5OS" $(FLOPPY)

run: all
	dd conv=notrunc if=$(BOOT) of=$(FLOPPY) bs=1 skip=62 seek=62
	sudo mount -o loop $(FLOPPY) /mnt/
	sudo cp floppy_contents/* /mnt/
	sudo umount /mnt/
	qemu-system-i386 -drive file=$(FLOPPY),index=0,media=disk,format=raw

debug_kernel: $(KERNEL)
	ndisasm -b 16 -p intel $<

files:
	/bin/ls -la floppy_contents/

clean:
	#rm -f $(FLOPPY)
	rm -f obj/*
	rm -f $(KERNEL)
