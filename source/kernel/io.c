#include "io.h"

void putchar(char c) {
	c = c;
	#asm
		push ax
		push bx

		mov ah, #0x0e  //teletype print
		mov al, [bp+0x4] //use base pointer to get char
		xor bx, bx     //write to 0th page
		int 0x10

		pop bx
		pop ax
	#endasm
}

void print(char *str) {
	while(*str) {
		putchar(*str);
		str++;
	}
}

void puts(char *str) {
	print(str);
	putchar('\r');
	putchar('\n');
}

void screen_clear() {
	#asm
		push ax

		mov ah, #0x00
		mov al, #0x03
		int 0x10

		pop ax
	#endasm
}
