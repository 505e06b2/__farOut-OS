#ifndef _START_H
#define _START_H

#include "stdint.h"

#define FILENAME_SIZE (8+3)

#pragma pack(push, 2) //this should be done automatically by the compiler, but it is very important to have stack alignment
typedef struct start_data_s {
	int argc; //min of 0, max of the size of argv (3)
	char argv[3][FILENAME_SIZE];
	uint8_t boot_drive_id;
} start_data_t;
#pragma pack(pop)

#endif
