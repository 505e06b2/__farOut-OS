#ifndef _LIB_H
#define _LIB_H

#include "stdint.h"

#define NULL 0

char *itoa(uint16_t, char *, uint8_t);

void halt();
void exec(char *);

#endif
