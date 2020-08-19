#include "io.h"
#include "lib.h"

/* Quick Inline ASM tips for BCC *
- When specifying a number, put # before it -> #0x0a
- When using an interrupt, don't use #
- Otherwise, it's just intel/NASM
*/

unsigned char drive_id;

#asm
	//need ds to get pointers to work
	push ax
	mov ax, cs
	mov ds, ax
	pop ax
#endasm

void main() {
	asm "mov _drive_id, dl"; //save drive id
	//screen_clear();
	puts("Done!");
	print("Drive ID => 0x"); print(itoa(drive_id, 16));

	while(1) asm "hlt";
}

