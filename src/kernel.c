#include "io.h"
#include "fs.h"
#include "stdlib.h"
#include "stdint.h"

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

asm (//Must do this in every program if wanting to read from boot drive
	"mov boot_drive_id, dl;"
);

void _start() {
	bpb_t bpb_info;
	file_info_t file_info;

	screen_clear();
	puts("Booted into fiveOS");
	printf("Drive ID => 0x%2x\r\n", boot_drive_id);// puts(itoa(boot_drive_id, text_buffer, 16));

	readSector(boot_drive_id, (uint8_t *)&bpb_info, 0);

	if(getBPB(boot_drive_id, &bpb_info) == NULL) {
		puts("!!!Could not parse boot sector!!!");
		halt();
	}
	puts("Loaded BPB :)");

	//print_root_directory(boot_drive_id, &bpb_info);

	if(find_file_info(boot_drive_id, &bpb_info, &file_info, "KERNEL  COM") == NULL) {
		puts("!!!Could not find \"KERNEL  COM\" on floppy!!!");
		halt();
	}

	printf("\"%8s.%3s\" - %d bytes\r\n", file_info.name, file_info.name+8, file_info.size);

	puts("Press any key to launch shell");
	getchar();

	puts("...");
	panic();
}

