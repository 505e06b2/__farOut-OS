#ifndef _LIB_H
#define _LIB_H

#include <stdint.h>

#include "io.h"

#define NULL 0

#define PHYSICAL_ADDRESS_TO_FAR_POINTER(x) ( ((x & 0xffff0000) << 12) + (x & 0x0000ffff) )
#define PHYSICAL_ADDRESS_TO_SEGMENT(x) ( x >> 4 )
#define PHYSICAL_ADDRESS_TO_OFFSET(x) ( x & 0xf )

char __far *itoa(uint16_t, char __far *, uint8_t);

void halt();
void panic();

#endif
