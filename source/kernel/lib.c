#include "lib.h"

char *itoa(int value, int base) {
	static char buffer[6];
	char *ptr = buffer + sizeof(buffer);
	*ptr = '\0';
	ptr--;

	do {
        *--ptr = "0123456789abcdef"[value % base];
        value /= base;
    } while(value != 0);

	return ptr;
}
