#include "io.h"

void putchar(char c) {
	c = c;
	#asm
		push ax
		push bx

		mov ah, #0x0e  //teletype print
		mov al, [bp+0x04] //truncate the value to al
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

char getchar(char ret) { //so that it can be used with bp - need to figure out a way to access stack vars
	ret = ret;
	#asm
		push ax

		mov ah, #0x00
		int 0x16
		mov [bp+0x04], al

		pop ax
	#endasm
	return ret;
}

char *gets(char *ret) {
	char *ptr = ret;
	char current_char;

	while((current_char = getchar()) != '\r') {
		if(current_char == '\010') { //backspace
			if(ptr == ret) continue;
			putchar('\010');
			putchar(' ');
			putchar('\010');
			ptr--;
			continue;
		}
		*ptr = current_char;
		ptr++;
		putchar(current_char);
	}

	*ptr = '\0';
	putchar('\r');
	putchar('\n');

	return ret;
}
