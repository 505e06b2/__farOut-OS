#include "stdint.h"

void main();

//these can't be on the stack
uint16_t previous_stack_pointer;
uint16_t previous_segment;

void __far _start() {
	asm volatile (
		"cli;"
		"mov ax, cs;"
		"mov ds, ax;" //need this set to alter the globals

		"mov previous_stack_pointer, sp;"
		"mov previous_segment, ss;"

		"mov es, ax;"
		"mov ss, ax;"

		"mov ax, sp;"
		"mov ax, 0xfffc;"
		"mov sp, ax;" //set stack pointer to bottom of segment (4-byte aligned)
		"sti;"
	);

	main();

	asm volatile (
		"cli;"
		"mov ax, previous_segment;"
		"mov sp, previous_stack_pointer;"

		"mov es, ax;"
		"mov ss, ax;"

		"mov ds, ax;"
		"sti;"
	);
}
