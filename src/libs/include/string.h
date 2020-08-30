#ifndef _STRING_H
#define _STRING_H

#include "stdint.h"

size_t strlen(const char *);
int strncmp(const char *, const char *, size_t);

void *memcpy(void *, const void *, size_t);

#endif
