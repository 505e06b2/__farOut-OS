#include "io.h"
#include "stdlib.h"

/* Quick Inline ASM tips for BCC *
- When specifying a number, put # before it -> #0x0a
- When using an interrupt, don't use #
- Otherwise, it's just intel/NASM

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
- For COM files, like this kernel, the Data and Code segments (DS/CS) are the same, while the stack sits elsewhere
*/

#define STACK_ADDRESS 0x00010500 //64k from 0x500 - 16 byte aligned

#define STACK_SEGMENT (STACK_ADDRESS >> 4)
#define STACK_OFFSET  (STACK_ADDRESS & 0xf)

#asm
	cli
	//data segment must match code segment, as we use the COM executable format
	mov ax, cs
	mov ds, ax
	//set the stack to the above offset
    mov ax, #STACK_SEGMENT
    mov ss, ax
    mov sp, #STACK_OFFSET
	sti
#endasm

unsigned char boot_drive_id;
#asm //Must do this in every program if wanting to read from boot drive
	mov _boot_drive_id, dl
#endasm

void main() {
	char buffer[512];

	screen_clear();
	puts("Booted into fiveOS");
	puts("Launching shell...");

	print("Drive ID => 0x"); puts(itoa(boot_drive_id, buffer, 16));

	readSector(boot_drive_id, buffer, 0);
	if(buffer[510] == 0x55 && buffer[511] == 0xaa) puts("Readsector successful");
	else puts("!!!Readsector unsuccessful!!!");

	while(1) asm "hlt";
}

