#include "io.h"
#include "fs.h"
#include "utils.h"
#include "stdint.h" //my version uses uin16_t as size_t

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

asm ( //drives will be handled by the kernel, but this one is important to know
	"mov boot_drive_id, dl;"
);

void _start() {
	bpb_t bpb_info;
	file_info_t file_info;
	volatile uint8_t __far *byte;

	clearScreen();
	//printString("Booted into fiveOS\r\n");
	//printf("Drive ID => 0x%2x\r\n", boot_drive_id);

	if(getBPB(boot_drive_id, &bpb_info) == NULL) {
		printString("Could not parse boot sector!\r\n");
		halt();
	}
	//printString("Loaded BPB :)\r\n");

	if(findFileInfo(boot_drive_id, &bpb_info, &file_info, "KERNEL  COM") == NULL) {
		printString("Could not find \"KERNEL  COM\" on disk!\r\n");
		//printString("This may have something to do with not differentiating HDD/Floppy\r\n");
		halt();
	}

	printf("\"%8s.%3s\" - %d bytes\r\n", file_info.name, file_info.name+8, file_info.size);
	copyFileContents(boot_drive_id, PHYSICAL_ADDRESS_TO_SEGMENT(0x00060000), file_info.low_cluster_number);

	byte = (volatile uint8_t __far *)(PHYSICAL_ADDRESS_TO_FAR_POINTER(0x00060000));
	if(byte[0] == 0xeb && byte[1] == 0x3c && byte[2] == 0x90 && byte[3] == 0x6d) {
		//printString("First bytes of boot drive read successfully\r\n");
	} else {
		printString("First bytes of boot drive not read properly!\r\n");
		halt();
	}

	//printString("...\r\n");
	panic();
}

