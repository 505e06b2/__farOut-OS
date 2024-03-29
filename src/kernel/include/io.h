#ifndef _IO_H
#define _IO_H

#include <stdint.h>
#include <stddef.h>

#define SECTOR_SIZE 512

#define SEGMENT_TO_FAR_POINTER(x) ( (x & 0xffffffff) << 16 )
#define PHYSICAL_ADDRESS_TO_FAR_POINTER(x) ( ((x & 0xffff0000) << 12) + (x & 0x0000ffff) )
#define PHYSICAL_ADDRESS_TO_SEGMENT(x) ( x >> 4 )
#define PHYSICAL_ADDRESS_TO_OFFSET(x) ( x & 0xf )
#define KERNEL_POINTER_TO_FAR_POINTER(x) ( 0x70000000 + x )

#define FLOPPY_SECTORS_PER_TRACK 18
#define FLOPPY_NUMBER_OF_HEADS 2
#define HDD_SECTORS_PER_TRACK 0b00111111 //this seems to be consistent for hdds
#define HDD_NUMBER_OF_HEADS 0xff

void clearScreen();
void printChar(const char);
void printString(const char __far *);
int compareStringN(const volatile char __far *, const volatile char __far *, size_t); //strncmp
volatile void __far *copyMemory(volatile void __far *, const volatile void __far *, size_t); //memcpy
size_t copyStringMemory(volatile char __far *, const volatile char __far *); //strcpy
volatile void __far *setMemory(volatile void __far *, int, size_t); //memset


uint8_t *readSector(uint8_t, uint8_t *, uint16_t);
void readSectorFar(uint8_t, uint16_t, uint16_t, uint16_t);

#endif
