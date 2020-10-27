#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>

#include "stdio.h"

char * __far itoa(uint16_t, char *, uint8_t);
char * internal_itoa(uint16_t, char *, uint8_t);

#endif
