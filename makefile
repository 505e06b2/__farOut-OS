BOOT=obj/bootloader.bin
KERNEL=contents/kernel.bin
FLOPPY=floppy.raw

CFLAGS=-0 -W -Md -ansi -c -Isource/kernel/include/
LDFLAGS=-d -M

.PHONY: run init clean

all: $(BOOT) $(KERNEL)

$(BOOT): source/bootloader/boot.nasm
	nasm $^ -f bin -o $@

$(KERNEL): obj/main.o obj/io.o obj/lib.o
	ld86 $(LDFLAGS) $^ -L/usr/lib/bcc/ -lc -o $@

obj/%.o: source/kernel/%.c
	bcc $(CFLAGS) $< -o $@

init:
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	mkfs.fat -F 12 -D 0 -n "5OS" $(FLOPPY)

run: all
	dd conv=notrunc if=$(BOOT) of=$(FLOPPY) bs=1 skip=62 seek=62
	sudo mount -o loop $(FLOPPY) /mnt/
	sudo cp contents/* /mnt/
	sudo umount /mnt/
	qemu-system-i386 -drive file=$(FLOPPY),index=0,media=disk,format=raw

clean:
	rm -f $(FLOPPY)
	rm -f obj/*
	rm -f contents/*
