#include "stdio.h"

static void _print(const char __far *str) {
	while(*str) {
		putchar(*str);
		str++;
	}
}

void __far putchar(const char c) {
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

void __far puts(const char *str) {
	_print(str); //replace with kernel call
	putchar('\r');
	putchar('\n');
}

void __far printf(const char *format, ...) {
	char text_buffer[30]; //for use with converting ints
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

			if(internal_isdigit(*(format+1))) {
				fmt_add = 2;
				wanted_size = *(format+1) - '0'; // -'0' to turn the char into an int
			}


			switch(*(format + fmt_add)) {
				case 'd':
					expanded_start = internal_itoa(va_arg(args, int), text_buffer, 10);
					for(int i = wanted_size - internal_strlen(expanded_start); i > 0; i--) putchar('0');
					_print(expanded_start);
					format += fmt_add;
					break;

				case 'x':
					expanded_start = internal_itoa(va_arg(args, unsigned int), text_buffer, 16);
					for(int i = wanted_size - internal_strlen(expanded_start); i > 0; i--) putchar('0');
					_print(expanded_start);
					format += fmt_add;
					break;

				case 's':
					if(wanted_size == 0) {
						_print(va_arg(args, char *));
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


char __far getchar() {
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

char * __far gets(char *ret) {
	char *ptr = ret;
	char current_char;

	while((current_char = getchar()) != '\r') {
		if(current_char == '\010') { //backspace
			if(ptr == ret) continue; //already at inital cursor location
			putchar('\010'); //move back
			putchar(' '); //clear char
			putchar('\010'); //move back
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
