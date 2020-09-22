#include "io.h"

void printChar(const char c) {
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

void printString(const char __far *str) {
	while(*str) {
		printChar(*str);
		str++;
	}
}

void clearScreen() {
	asm volatile (
		"mov ah, 0x00;"
		"mov al, 0x03;"
		"int 0x10;"
		: //out
		: //in
		: "ax" //clobber
	);
}


char getChar() {
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


char __far *getString(char __far *ret) {
	char __far *ptr = ret;
	char current_char;

	while((current_char = getchar()) != '\r') {
		if(current_char == '\010') { //backspace
			if(ptr == ret) continue; //already at inital cursor location
			putchar('\010'); //move back
			putchar(' '); //clear char
			putchar('\010'); //move back
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

static void _getCHS(chs_t *chs, uint16_t lba) {
	chs->cx.segments.cylinder = lba / H_x_SPT;
	chs->dh.segments.head = (lba % H_x_SPT) / SECTORS_PER_TRACK;
	chs->cx.segments.sector = (lba % H_x_SPT) % SECTORS_PER_TRACK;
}

uint8_t *readSector(uint8_t drive_id, uint8_t *buffer, uint16_t lba) {
	register uint16_t *stack_segment asm ("ss"); //the buffer should be on the stack -> buffer[SECTOR_SIZE]
	readSectorFar(drive_id, (uint16_t)stack_segment, (uint16_t)buffer, lba);
	return buffer;
}

void readSectorFar(uint8_t drive_id, uint16_t segment, uint16_t pointer, uint16_t lba) {
	chs_t chs;

	_getCHS(&chs, lba);

	asm volatile (
		"mov es, %0;"
		"mov bx, %1;"

		"mov cx, %3;"
		"inc cl;" //sector is 1 indexed

		"mov dh, %4;"
		"mov dl, %2;"

		"mov al, 1;"
		"mov ah, 0x02;"
		"int 0x13;"
		:
		: "g" (segment), "g" (pointer), "g" (drive_id), "g" (chs.cx.raw), "g" (chs.dh.raw)
		: "ax", "bx", "cx", "dx", "cc", "memory", "es"
	);
}

