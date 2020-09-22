#ifndef _STRING_H
#define _STRING_H

#include "stdint.h"

size_t strlen(const volatile char __far *);
int strncmp(const volatile char __far *, const volatile char __far *, size_t);

volatile void __far *memcpy(volatile void __far *, const volatile void __far *, size_t);

#endif
