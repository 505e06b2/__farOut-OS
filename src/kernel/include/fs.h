#ifndef _FS_H
#define _FS_H

#include "io.h"
#include "utils.h"
#include "stdint.h"

enum file_info_attributes {
	READ_ONLY = 0x01,
	HIDDEN = 0x02,
	SYSTEM = 0x04,
	VOLUME_ID = 0x08,
	DIRECTORY = 0x10,
	ARCHIVE = 0x20,
	LONG_FILENAME = READ_ONLY | HIDDEN | SYSTEM | VOLUME_ID
};

typedef struct drive_info_s {
	uint8_t id; //maybe add a serial to determine if replaced in the drive
	struct {
		uint8_t amount; //number of fats on the system - 2 for FAT12
		uint16_t size; //in sectors - just for one
		uint16_t start; //first sector
	} fat;
	struct {
		uint16_t size;
		uint16_t start;
	} root_directory;
	struct {
		uint16_t start;
	} data;
} drive_info_t;

#pragma pack(1)

typedef struct bpb_s { //BIOS parameter block - this fits perfectly over a sector
	uint8_t jump[3];
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
	uint32_t volume_id;
	char label[11];
	char file_system[8];
	uint8_t bootstrap[448];
	uint16_t signature;
} bpb_t;

typedef union file_info_time_u {
	uint16_t raw;
	struct {
		uint8_t seconds: 5; //*2 for real value
		uint8_t minutes: 6;
		uint8_t hours: 5;
	} value;
} file_info_time_t;

typedef union file_info_date_u {
	uint16_t raw;
	struct {
		uint8_t day: 5;
		uint8_t month: 4;
		uint8_t year: 7; //+1980
	} value;
} file_info_date_t;

typedef struct file_info_s {
	uint8_t name[11]; // 8.3 -> no null terminator
	uint8_t attributes;
	uint8_t reserved;
	uint8_t creation_time_tenths_s;
	file_info_time_t creation_time;
	file_info_date_t creation_date;
	file_info_date_t accessed_date;
	uint16_t high_cluster_number;
	file_info_time_t modified_time;
	file_info_date_t modified_date;
	uint16_t low_cluster_number;
	uint32_t size;
} file_info_t;

typedef struct long_file_info_s { //long filename
	uint8_t order;
	uint16_t first_segment[5]; //change to wchar_t at some point?
	uint8_t attributes; //will always be 0x0F for long filename
	uint8_t entry_type;
	uint8_t checksum;
	uint16_t second_segment[6];
	uint16_t reserved;
	uint16_t third_segment[2];
} long_file_info_t;

#pragma pack()

drive_info_t *findDriveInfo(const uint8_t, drive_info_t *);
file_info_t *findFileInfo(const drive_info_t *, const char *, file_info_t *);
uint16_t copyFileContents(const drive_info_t *, const file_info_t *, uint16_t);

//helper function for lib
int __far mapFileToSegment(uint8_t, const char *, uint16_t);

#endif
