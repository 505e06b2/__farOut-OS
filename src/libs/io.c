#include "io.h"

void putchar(char c) { //c is stored in AX
	#asm
		push ax
		push bx

		mov ah, #0x0e  //teletype print
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


char getchar() {
	char ret = '`';
	#asm
		push ax

		mov ah, #0x00
		int 0x16
		mov [bp-0x5], al //set ret

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

char *readSector(unsigned char drive_id, unsigned char *buffer, unsigned char sector_id) {
	drive_id = drive_id; //force the creation of a scope, so that we can use bp
	//drive_id -> [bp+0x4]
	//buffer -> [bp+0x6]
	//sector_id -> [bp+0x8]
	#asm
		push ax
		push bx
		push cx
		push dx

		//essentially taken from fryy/io.c -> load_sectors
		mov ax, ss //ss, since the buffer should be on the stack -> buffer[512]
		mov es, ax
		mov bx, [bp+0x6] //buffer
		push bx
		mov ax, [bp+0x8] //sector id
		mov bl, #18
		div bl
		inc ah
		mov cl, ah
		mov ch, al
		shr ch, #1
		mov dh, al
		and dh, #1
		mov dl, [bp+0x4]
		pop bx
		_load_sectors_redo: mov ah, #2
		mov al, #1
		int 0x13
		jc _load_sectors_redo

		pop dx
		pop cx
		pop bx
		pop ax
	#endasm
	return buffer;
}


