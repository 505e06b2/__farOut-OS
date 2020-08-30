#include "stdlib.h"

static const char *hex_charset = "0123456789abcdefghijklmnopqrstuvwxyz";

//Make sure to use the return value, since the start of the buffer may not be filled
char *itoa(uint16_t value, char *buffer, uint8_t base) {
	size_t index = 5; //BUFFER MUST BE AT LEAST 6 (to fit "65535\0"). This should not be a problem, as we are converting int to str

	buffer[index] = '\0';

	do {
        buffer[--index] = hex_charset[value % base];
        value /= base;
    } while(value != 0);

	return &buffer[index];
}

void halt() {
	while(1) asm ("hlt");
}

void panic() {
	puts("Kernel tried to exit, halting.");
	halt();
}
