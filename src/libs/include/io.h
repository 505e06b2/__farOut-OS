#ifndef _IO_H
#define _IO_H

#include "stdlib.h"
#include "stdint.h"

//QEMU SEEMS WEIRD ABOUT THIS
#define SECTORS_PER_TRACK 0b00111111 //qemu sets this to the max?
#define NUMBER_OF_HEADS 0xff
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

void putchar(char);
void print(char *);
void puts(char *);

void screen_clear();

char getchar();
char *gets();

void _getCHS(chs_t *, uint16_t);
uint8_t *readSector(uint8_t, uint8_t *, uint16_t);

#endif
