#include "io.h"
#include "fs.h"
#include "task.h"
#include "utils.h"

#include "stdint.h" //my version uses uint16_t as size_t
#include "stdlib_farptrs.c"

/* Quick Inline ASM tips for BCC *
- When specifying a number, put # before it -> #0x0a
- When using an interrupt, don't use #
- Otherwise, it's just intel/NASM

Currently not even using BCC, but may come in handy in the future - GCC is infintely better though

= Research on 8086 =

Registers
- General Purpose (have upper and lower bytes)
    - AX -> Accumulator (arithmetic)
    - BX -> Base (values of offsets)
    - CX -> Counter (looping)
    - DX -> Data (multiplication)

- Indexing
    - SI -> Source Index (used in lodsb)
    - DI -> Destination Index (used in stosb)
    - BP -> Base Pointer (points to parameters passed by the stack)
    - SP -> Stack Pointer (points to last thing put onto the stack)

- Segment Registers
    - CS -> Code Segment
    - DS -> Data Segment
    - SS -> Stack Segment
    - ES -> Extra Segment (no hardware purpose)

- Other Registers
    - IP -> Instruction Pointer (CS:IP + points to current instruction)
    - FLAGS -> Flags/Status/Condition Code Register

Segments
- Max size of 64k (but also minimum size, since there are no hard boundries outside of this one)
- Each segment register points to the start of a 64k segment, so that the program can use more than 64k at once
- For COM files, like this kernel, all segments (CS/DS/SS/ES) are the same
*/

/*
Depending on the size of the final kernel, there may be a need to split the stdlib into its own file
*/

asm (
	"cli;"
	//tiny memory model -> ds = ss = es = cs
	"mov ax, cs;"
	"mov ds, ax;"
	"mov es, ax;"
	"mov ss, ax;"

	"mov ax, 0xfffc;"
	"mov sp, ax;" //set stack pointer to bottom of segment (4-byte aligned)
	"sti;"
);

uint8_t boot_drive_id;
task_t current_task;

asm ( //this will be important to know
	"mov boot_drive_id, dl;"
);

void _start() {
	drive_info_t boot_drive_info;

	clearScreen();

	if(findDriveInfo(boot_drive_id, &boot_drive_info) == NULL) {
		printString("Could not parse boot sector!");
		halt();
	}

	{
		file_info_t file_info;

		if(findFileInfo(&boot_drive_info, "LIBC    COM", &file_info) == NULL) {
			printString("Could not find \"LIBC.COM\" on disk!"); //may have something to do with the CHS of HDD/Floppy not being accounted for
			halt();
		}

		copyFileContents(&boot_drive_info, &file_info, PHYSICAL_ADDRESS_TO_SEGMENT(0x00060000));
	}
	//stdlib should work now
	printf("Drive ID => 0x%2x\r\n", boot_drive_id);
	puts("TEXT String");

	puts("Launching small program at \"5000:0000\"");

	task_create(&boot_drive_info, 0x5000, "SHELL   COM", &current_task);

	printf("Program printed: \"");
	task_run(&current_task);
	printf("\"\r\n");

	printf("No errors detected :)\r\n");
	panic();
}

