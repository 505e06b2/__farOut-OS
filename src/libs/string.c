#include "string.h"

#pragma GCC push_options
#pragma GCC optimize ("Os", "no-peephole2") //far pointers don't like peephole2

size_t strlen(const volatile char __far *str) {
	size_t ret = 0;
	while(*str) {
		str++;
		ret++;
	}
	return ret;
}

int strncmp(const volatile char __far *original, const volatile char __far *compare, size_t max_length) {
	for(size_t i = 0; i < max_length && original[i] && compare[i]; i++) { //ensure '\0' is checked
		if(original[i] > compare[i]) return 1;
		if(original[i] < compare[i]) return -1;
	}
	return 0;
}

volatile void __far *memcpy(volatile void __far *destination, const volatile void __far *source, size_t number_of_bytes) {
	for(size_t i = 0; i < number_of_bytes; i++) {
		((volatile uint8_t __far *)destination)[i] = ((const volatile uint8_t __far *)source)[i];
	}
	return destination;
}

#pragma GCC pop_options
