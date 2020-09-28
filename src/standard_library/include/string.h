#ifndef _STRING_H
#define _STRING_H

#include "stdint.h"

size_t strlen(const volatile char *); //make __far at some point - need to understand how things work better
int __far strncmp(const volatile char *, const volatile char *, size_t);

void * __far memcpy(void *, const void *, size_t);

#endif
