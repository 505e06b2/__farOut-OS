#include "io.h"

void clearScreen() { //this just sets the video mode to 80x25 colour
	asm volatile (
		"mov ah, 0x00;"
		"mov al, 0x03;"
		"int 0x10;"
		: //out
		: //in
		: "ax" //clobber
	);
}

void printString(const char __far *str) {
	while(*str) {
		asm volatile (
			"mov ah, 0x0e;"  //teletype print
			"mov al, %0;"
			"xor bx, bx;"     //write to 0th page
			"int 0x10;"
			:
			: "r" (*str++)
			: "ax", "bx"
		);
	}
}

int compareStringN(const volatile char __far *original, const volatile char __far *compare, size_t max_length) {
	for(size_t i = 0; i < max_length && original[i] && compare[i]; i++) { //ensure '\0' is checked
		if(original[i] > compare[i]) return 1;
		if(original[i] < compare[i]) return -1;
	}
	return 0;
}

volatile void __far *copyMemory(volatile void __far *destination, const volatile void __far *source, size_t number_of_bytes) {
	for(size_t i = 0; i < number_of_bytes; i++) {
		((volatile uint8_t __far *)destination)[i] = ((const volatile uint8_t __far *)source)[i];
	}
	return destination;
}

uint8_t *readSector(uint8_t drive_id, uint8_t *buffer, uint16_t lba) {
	register uint16_t *stack_segment asm ("ss"); //the buffer should be on the stack -> buffer[SECTOR_SIZE]
	readSectorFar(drive_id, (uint16_t)stack_segment, (uint16_t)buffer, lba);
	return buffer;
}

void readSectorFar(uint8_t drive_id, uint16_t segment, uint16_t pointer, uint16_t lba) {
	uint16_t sectors_per_track = FLOPPY_SECTORS_PER_TRACK;
	uint16_t number_of_heads = FLOPPY_NUMBER_OF_HEADS;

	if(drive_id >= 0x80) {
		sectors_per_track = HDD_SECTORS_PER_TRACK;
		number_of_heads = HDD_NUMBER_OF_HEADS;
	}

	asm volatile (
		"mov ax, %2;" //LBA
		"mov es, %0;" //set segment

		"xor dx, dx;"
		"mov bx, %3;" //Divide by the sectors per track
		"div bx;"
		"mov cx, dx;"
		"inc cx;" //sectors are 1-indexed

		"xor dx, dx;"
		"mov bx, %4;" //Divide by the number of heads
		"div bx;"
		"mov dh, dl;"// Move the absolute head into dh

		"mov ch, al;"// Low 8 bits of absolute track
		"push cx;"
		"mov cl, 6;"// High 2 bits of absolute track
		"shl ah, cl;" //SHift Left
		"pop cx;"
		"or cl, ah;"// Now cx is set with respective track and sector numbers

		"mov dl, %5;" // set drive

		"mov ax, 0x0201;"//                              ; Read Sectors func of int 13h, read one sector

		"mov bx, %1;" //set pointer
		"int 0x13;"
		:
		: "r" (segment), "g" (pointer), "g" (lba), "g" (sectors_per_track), "g" (number_of_heads), "g" (drive_id)
		: "ax", "bx", "cx", "dx", "cc", "memory", "es"
	);
}

