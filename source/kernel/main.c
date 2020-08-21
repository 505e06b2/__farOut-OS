#include "io.h"
#include "lib.h"

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

unsigned char drive_id;

#asm
	//data segment must match code segment, as we use the COM executable format - stack is set by stage 1
	push ax

	mov ax, cs
	mov ds, ax

	pop ax
#endasm

void main() {
	asm "mov _drive_id, dl";
	char buffer[30];

	puts("Done!");

	screen_clear();
	puts("We're in real mode, with type sizes of:");
	print("    - double: "); puts(itoa(sizeof(double), buffer, 10));
	print("    - float: "); puts(itoa(sizeof(float), buffer, 10));
	print("    - long: "); puts(itoa(sizeof(long), buffer, 10));
	print("    - int: "); puts(itoa(sizeof(int), buffer, 10));
	print("    - char: "); puts(itoa(sizeof(char), buffer, 10));
	print("\r\n");
	print("Drive ID => 0x"); puts(itoa(drive_id, buffer, 16));
	print("Type Something: "); gets(buffer);
	print("Got => "); puts(buffer);

	while(1) asm "hlt";
}

