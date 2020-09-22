#include "stdio.h"

static void _print(const char __far *str) {
	while(*str) {
		printChar(*str); //replace with kernel call
		str++;
	}
}

void putchar(const char c) {
	printChar(c); //replace with kernel call
}

void puts(const char __far *str) {
	printString(str); //replace with kernel call
	putchar('\r');
	putchar('\n');
}

void printf(const char __far *format, ...) {
	char text_buffer[30]; //for use with expanding chars
	int fmt_add;
	int wanted_size;
	char __far *expanded_start;

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
					_print(expanded_start);
					format += fmt_add;
					break;

				case 'x':
					expanded_start = itoa(va_arg(args, int16_t), text_buffer, 16);
					for(int i = wanted_size - strlen(expanded_start); i > 0; i--) putchar('0');
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


char getchar() {
	return getChar(); //replace with kernel call
}

char __far *gets(char __far *str) {
	return getString(str); //replace with kernel call
}
