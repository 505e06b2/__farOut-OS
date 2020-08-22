BOOT=obj/bootloader.bin
LIBS=obj/lib_io.o obj/lib_stdlib.o obj/lib_bcc_int.o
KERNEL=floppy_contents/kernel.com
FLOPPY=floppy.img

CFLAGS=-0 -W -Md -ansi -c -Isrc/libs/include/
LDFLAGS=-d -M

.PHONY: run init clean debug_kernel

all: $(BOOT) $(KERNEL)

$(BOOT): src/bootloader.nasm
	nasm $^ -f bin -o $@

$(KERNEL): obj/kernel.o $(LIBS)
	ld86 $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	bcc $(CFLAGS) $< -o $@

# Must use a header file for each c file - makes sure that if the header is changed in any lib, the lib is recompiled
obj/lib_%.o: src/libs/%.c src/libs/include/%.h
	bcc $(CFLAGS) $< -o $@

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
