#include "io.h"


void putchar(char c) { //c is stored in AX
	asm volatile (
		"mov ah, 0x0e;"  //teletype print
		"mov al, %0;"
		"xor bx, bx;"     //write to 0th page
		"int 0x10;"
		:
		: "r" (c)
		: "ax", "bx"
	);
}

void print(char *str) {
	while(*str) {
		putchar(*str);
		str++;
	}
}

void puts(char *str) {
	print(str);
	putchar('\r');
	putchar('\n');
}


void screen_clear() {
	asm volatile (
		"mov ah, 0x00;"
		"mov al, 0x03;"
		"int 0x10;"
		: //out
		: //in
		: "ax" //clobber
	);
}


char getchar() {
	char ret;
	asm volatile (
		"mov ah, 0x00;"
		"int 0x16;"
		"mov %0, al;" //set ret
		: "=r" (ret)
		:
		: "ax"
	);
	return ret;
}


char *gets(char *ret) {
	char *ptr = ret;
	char current_char;

	while((current_char = getchar()) != '\r') {
		if(current_char == '\010') { //backspace
			if(ptr == ret) continue;
			putchar('\010');
			putchar(' ');
			putchar('\010');
			ptr--;
			continue;
		}
		*ptr = current_char;
		ptr++;
		putchar(current_char);
	}

	*ptr = '\0';
	putchar('\r');
	putchar('\n');

	return ret;
}

uint8_t *readSector(uint8_t drive_id, uint8_t *buffer, uint16_t sector_start, uint8_t sector_count) {
	asm volatile (
		//essentially taken from fryy/io.c -> load_sectors
		"mov ax, ss;" //ss, since the buffer should be on the stack -> buffer[512]
		"mov es, ax;"
		"mov bx, %0;" //buffer
		"push bx;"
		"mov ax, 0;" //sector start
		"mov bl, 18;"
		"div bl;"
		"inc ah;"
		"mov cl, ah;"
		"mov ch, al;"
		"shr ch, 1;"
		"mov dh, al;"
		"and dh, 1;"
		"mov dl, %3;" //drive id
		"pop bx;"
		"_load_sectors_redo: mov ah, 2;" //set Read Sector
		"mov al, %4;" //sector count
		"int 0x13;"
		"jc _load_sectors_redo;"
		: "+g" (buffer)
		: "g" (buffer), "g" (sector_start), "g" (drive_id), "g" (sector_count)
		: "ax", "bx", "cx", "dx", "cc", "memory"
	);
	return buffer;
}

bpb_t *getBPB(uint8_t drive_id, uint8_t *buffer) {
	bpb_t *bpb = (bpb_t *)buffer;
	readSector(drive_id, buffer, 0, 1);

	if(bpb->signature[0] == 0x55 && bpb->signature[1] == 0xaa) {
		return bpb;
	} else {
		return NULL;
	}
}
