#include "string.h"

size_t strlen(const char *str) {
	size_t ret = 0;
	while(*str) {
		str++;
		ret++;
	}
	return ret;
}

int strncmp(const char *original, const char *compare, size_t max_length) {
	for(size_t i = 0; i < max_length && original[i] && compare[i]; i++) { //ensure '\0' is checked
		if(original[i] > compare[i]) return 1;
		if(original[i] < compare[i]) return -1;
	}
	return 0;
}

void *memcpy(void *destination, const void *source, size_t number_of_bytes) {
	for(size_t i = 0; i < number_of_bytes; i++) {
		((uint8_t *)destination)[i] = ((uint8_t *)source)[i];
	}
	return destination;
}
