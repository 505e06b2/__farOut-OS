#ifndef _IO_H
#define _IO_H

#include <stdarg.h>

#include "stdint.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"

#define SECTOR_SIZE 512

//USE THIS FOR HDD
/*
#define SECTORS_PER_TRACK 0b00111111 //qemu sets this to the max?
#define NUMBER_OF_HEADS 0xff
*/

//FLOPPY SETTINGS
#define SECTORS_PER_TRACK 18
#define NUMBER_OF_HEADS 2
#define H_x_SPT (NUMBER_OF_HEADS * SECTORS_PER_TRACK)

typedef struct chs_s {
	union {
		uint16_t raw;
		struct {
			uint8_t sector: 6;
			uint16_t cylinder: 10;
		} segments;
	} cx;
	union {
		uint8_t raw;
		struct {
			uint8_t head;
		} segments;
	} dh;
} chs_t;

void putchar(const char);
void print(const char *);
void printf(const char *, ...);
void puts(const char *);

void clearScreen();

char getchar();
char *gets();

uint8_t *readSector(uint8_t, uint8_t *, uint16_t);
void readSectorFar(uint8_t, uint16_t, uint16_t, uint16_t);

#endif
