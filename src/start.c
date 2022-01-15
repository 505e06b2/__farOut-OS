#include "start.h"

void main();

//these can't be on the stack since it gets moved
uint16_t previous_stack_pointer;
uint16_t previous_stack_segment;
uint16_t previous_base_pointer;

program_args_t *start_data = (program_args_t *)(0xffff + 1 - sizeof(program_args_t));

//need to be able to pass args from the kernel somehow, the stack is in the wrong segment here though
void __far _start() {
	asm volatile (
		"cli;"
		"mov ax, cs;"
		"mov ds, ax;" //need this set to alter the global variables

		"mov previous_stack_pointer, sp;"
		"mov previous_base_pointer, bp;"
		"mov previous_stack_segment, ss;"

		"mov es, ax;"
		"mov ss, ax;"

		"mov ax, 0xffff;"
		"mov bp, ax;" //set base pointer to end of start_data so it can be used as parameters (might not be necessary)

		"mov ax, start_data;"
		"mov sp, ax;" //set stack pointer to top of segment, under the start data which should be 2-byte aligned
		"sti;"

		"call main;"
		//don't need to restore stack since it will be discarded

		"cli;"
		"mov sp, previous_stack_pointer;"
		"mov bp, previous_base_pointer;"

		"mov ax, previous_stack_segment;"
		"mov ss, ax;"
		"mov es, ax;"
		"mov ds, ax;"
		"sti;"
		:
		:
		: "ax", "memory", "bp", "es", "ss", "ds"
	);
}
