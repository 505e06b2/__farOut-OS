#ifndef _IO_H
#define _IO_H

#include "stdlib.h"
#include "stdint.h"

#pragma pack(1)
typedef struct { //BIOS parameter block
	uint8_t initial_bootstrap[3];
	char description[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fats;
	uint16_t root_directory_entries;
	uint16_t total_sectors; //if set to 0, use alternative
	uint8_t media_descriptor;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t heads;
	uint32_t hidden_sectors;
	uint32_t alternative_total_sectors; //able to hold more, so this is used if the value is too big
	uint8_t drive_number;
	uint8_t windows_flags;
	uint8_t extended_boot_record_signature;
	uint32_t serial_number;
	char label[11];
	char file_system[8];
	uint8_t bootstrap[448];
	uint8_t signature[2];
} bpb_t;
#pragma pack()

void putchar(char);
void print(char *);
void puts(char *);

void screen_clear();

char getchar();
char *gets();

uint8_t *readSector(uint8_t, uint8_t *, uint16_t, uint8_t);
bpb_t *getBPB(uint8_t, uint8_t *);

#endif
