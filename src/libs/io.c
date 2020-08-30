#include "io.h"

void putchar(const char c) { //c is stored in AX
	asm volatile (
		"mov ah, 0x0e;"  //teletype print
		"mov al, %0;"
		"xor bx, bx;"     //write to 0th page
		"int 0x10;"
		:
		: "r" (c)
		: "ax", "bx"
	);
}

void print(const char *str) {
	while(*str) {
		putchar(*str);
		str++;
	}
}

void printf(const char *format, ...) {
	char text_buffer[30]; //for use with expanding chars
	int fmt_add;
	int wanted_size;
	char *expanded_start;

	va_list args;
    va_start(args, format);

	for(; *format; format++) {
		if(*format == '%') {
			fmt_add = 1;
			wanted_size = 0;

			switch(*(format+1)) { //could be kept in an if for now, but helps separate the different functions
				case 'c':
					putchar(va_arg(args, int));
					format++;
					continue; //skip the rest
			}

			if(isdigit(*(format+1))) {
				fmt_add = 2;
				wanted_size = *(format+1) - '0'; // -'0' to turn the char into an int
			}

			switch(*(format + fmt_add)) {
				case 'd':
					expanded_start = itoa(va_arg(args, int16_t), text_buffer, 10);
					for(int i = wanted_size - strlen(expanded_start); i > 0; i--) putchar('0');
					print(expanded_start);
					format += fmt_add;
					break;

				case 'x':
					expanded_start = itoa(va_arg(args, int16_t), text_buffer, 16);
					for(int i = wanted_size - strlen(expanded_start); i > 0; i--) putchar('0');
					print(expanded_start);
					format += fmt_add;
					break;

				case 's':
					if(wanted_size == 0) {
						print(va_arg(args, char *));
					} else {
						char *ptr = va_arg(args, char *);
						for(; wanted_size && *ptr; wanted_size--) putchar(*ptr++);
					}
					format += fmt_add;
					break;

				default:
					putchar(*format);
			}
			continue;
		}
		putchar(*format);
	}

	va_end(args);
}

void puts(const char *str) {
	print(str);
	putchar('\r');
	putchar('\n');
}


void screen_clear() {
	asm volatile (
		"mov ah, 0x00;"
		"mov al, 0x03;"
		"int 0x10;"
		: //out
		: //in
		: "ax" //clobber
	);
}


char getchar() {
	char ret;
	asm volatile (
		"mov ah, 0x00;"
		"int 0x16;"
		"mov %0, al;" //set ret
		: "=r" (ret)
		:
		: "ax"
	);
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

static void _getCHS(chs_t *chs, uint16_t lba) {
	chs->cx.segments.cylinder = lba / H_x_SPT;
	chs->dh.segments.head = (lba % H_x_SPT) / SECTORS_PER_TRACK;
	chs->cx.segments.sector = (lba % H_x_SPT) % SECTORS_PER_TRACK;
}

uint8_t *readSector(uint8_t drive_id, uint8_t *buffer, uint16_t lba) {
	chs_t chs;
	_getCHS(&chs, lba);

	asm volatile (
		"mov ax, ss;" //ss -> es, since the buffer should be on the stack -> buffer[512]
		"mov es, ax;"
		"mov bx, %0;"

		"mov cx, %2;"
		"inc cl;" //sector is 1 indexed

		"mov dh, %3;"
		"mov dl, %1;"

		"mov al, 1;"
		"mov ah, 0x02;"
		"int 0x13;"
		:
		: "g" (buffer), "g" (drive_id), "g" (chs.cx.raw), "g" (chs.dh.raw)
		: "ax", "bx", "cx", "dx", "cc", "memory", "es"
	);
	return buffer;
}

