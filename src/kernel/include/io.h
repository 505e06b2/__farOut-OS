#ifndef _IO_H
#define _IO_H

#include "stdint.h"

#define SECTOR_SIZE 512

#define FLOPPY 1

#if FLOPPY //(-fda)
	#define SECTORS_PER_TRACK 18
	#define NUMBER_OF_HEADS 2
#else //HDD (-hda)
	#define SECTORS_PER_TRACK 0b00111111 //this seems to be consistent for hdds
	#define NUMBER_OF_HEADS 0xff
#endif

void clearScreen();

uint8_t *readSector(uint8_t, uint8_t *, uint16_t);
void readSectorFar(uint8_t, uint16_t, uint16_t, uint16_t);

#endif
