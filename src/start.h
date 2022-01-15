#ifndef _START_H
#define _START_H

#include "stdint.h"

#define FILENAME_SIZE (8+3)

#pragma pack(push, 2) //this should be done automatically by the compiler, but it is very important to have stack alignment
typedef struct program_args_s {
	char *argv;
	uint8_t boot_drive_id;
} program_args_t;
#pragma pack(pop)

#endif
