#include "string.h"

size_t __far strlen(const volatile char *str) {
	size_t ret = 0;
	while(*str) {
		str++;
		ret++;
	}
	return ret;
}

size_t internal_strlen(const volatile char *str) {
	return strlen(str);
}

int __far strncmp(const volatile char *original, const volatile char *compare, size_t max_length) {
	for(size_t i = 0; i < max_length && original[i] && compare[i]; i++) { //ensure '\0' is checked
		if(original[i] > compare[i]) return 1;
		if(original[i] < compare[i]) return -1;
	}
	return 0;
}

int __far strcmp(const volatile char *original, const volatile char *compare) {
	for(size_t i = 0; original[i] && compare[i]; i++) { //ensure '\0' is checked
		if(original[i] > compare[i]) return 1;
		if(original[i] < compare[i]) return -1;
	}
	return 0;
}

void * __far memcpy(void *destination, const void *source, size_t number_of_bytes) {
	for(size_t i = 0; i < number_of_bytes; i++) {
		((uint8_t *)destination)[i] = ((const uint8_t *)source)[i];
	}
	return destination;
}

void * __far memset(void *destination, int value, size_t number_of_bytes) {
	for(size_t i = 0; i < number_of_bytes; i++) {
		((uint8_t *)destination)[i] = (uint8_t)value;
	}
	return destination;
}

char * __far strchr(char *str, int c) {
	for(; *str; str++) {
		if(*str == c) return str;
	}
	return NULL;
}
