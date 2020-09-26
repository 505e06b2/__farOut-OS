#include "io.h"

#include "stdio.h"

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

uint8_t *readSector(uint8_t drive_id, uint8_t *buffer, uint16_t lba) {
	register uint16_t *stack_segment asm ("ss"); //the buffer should be on the stack -> buffer[SECTOR_SIZE]
	readSectorFar(drive_id, (uint16_t)stack_segment, (uint16_t)buffer, lba);
	return buffer;
}

void readSectorFar(uint8_t drive_id, uint16_t segment, uint16_t pointer, uint16_t lba) {
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
		: "r" (segment), "g" (pointer), "g" (lba), "g" (SECTORS_PER_TRACK), "g" (NUMBER_OF_HEADS), "g" (drive_id)
		: "ax", "bx", "cx", "dx", "cc", "memory", "es"
	);
}

