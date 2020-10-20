#include "stdlib.h"

//const char hex_charset[] = "0123456789abcdefghijklmnopqrstuvwxyz";

//Make sure to use the return value, since the start of the buffer may not be filled
char * __far itoa(uint16_t value, char *buffer, uint8_t base) {
	size_t index;
	char hex_charset[16];

	//uint16_t previous_segment;
	/*asm volatile ( //allow access to hex_charset in the data segment - THIS IS HAVING ISSUES FOR THE MOMENT - WORKAROUND BELOW
		"mov ax, ds;"
		"mov %0, ax;"
		"mov ax, cs;"
		"mov ds, ax;"
		: "=g" (previous_segment)
		:
		: "ax", "ds"
	);*/

	//workaround
	for(size_t i = 0; i < 11; i++) hex_charset[i] = (i+48);
	for(size_t i = 0; i < 6; i++) hex_charset[i+10] = (i+97);


	index = 5; //BUFFER MUST BE AT LEAST 6 (to fit "65535\0"). This should not be a problem, as we are converting int to str
	buffer[index] = '\0';

	do {
        buffer[--index] = hex_charset[value % base];
        value /= base;
    } while(value != 0);

    /*asm volatile ( //reset ds to previous segment
		//"mov ax, %0;"
		"mov ds, %0;"
		:
		: "r" (previous_segment)
		: "ax"
	);*/

	return &buffer[index];
}

char *internal_itoa(uint16_t value, char *buffer, uint8_t base) {
	return itoa(value, buffer, base);
}
