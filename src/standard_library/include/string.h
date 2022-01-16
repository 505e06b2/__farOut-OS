#ifndef _STRING_H
#define _STRING_H

#include <stdint.h>
#include <stddef.h>

#include "stdlib.h"

size_t __far strlen(const volatile char *);
size_t internal_strlen(const volatile char *);
int __far strncmp(const volatile char *, const volatile char *, size_t);
int __far strcmp(const volatile char *, const volatile char *);

void * __far memcpy(void *, const void *, size_t);
void * __far memset(void *, int, size_t);

volatile char * __far strcpy(volatile char *, volatile char *);
char * __far strchr(char *, int);

#endif
