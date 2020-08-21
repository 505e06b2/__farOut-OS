#include "lib.h"

static char *hex_charset = "0123456789abcdefghijklmnopqrstuvwxyz";

char *itoa(unsigned int value, char *buffer, unsigned char base) {
	char *ptr = buffer + 6; //BUFFER MUST BE AT LEAST 6 (to fit "65535\0"). This should not be a problem, as we are converting int to str
	*ptr = '\0';
	ptr--;

	do {
        *--ptr = hex_charset[value % base];
        value /= base;
    } while(value != 0);

	return ptr;
}
