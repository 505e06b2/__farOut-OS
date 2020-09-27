#include "fs.h"

#include "stdio.h"

drive_info_t *findDriveInfo(const uint8_t drive_id, drive_info_t *drive_info) {
	bpb_t bootsector;
	readSector(drive_id, (void *)&bootsector, 0);

	if(bootsector.signature == 0xaa55) {
		drive_info->id = drive_id;

		drive_info->fat.amount = bootsector.fats;
		drive_info->fat.size = bootsector.sectors_per_fat;
		drive_info->fat.start = bootsector.reserved_sectors;

		drive_info->root_directory.size = ((bootsector.root_directory_entries * sizeof(file_info_t)) + (SECTOR_SIZE - 1)) / SECTOR_SIZE; //round to the biggest sector
		drive_info->root_directory.start = bootsector.reserved_sectors + (bootsector.fats * bootsector.sectors_per_fat);

		drive_info->data.start = drive_info->root_directory.start + drive_info->root_directory.size;

		return drive_info;
	} else {
		return NULL;
	}
}

file_info_t *findFileInfo(const drive_info_t *drive_info, const char *filename, file_info_t *file_info) {
	uint8_t root_dir_buffer[SECTOR_SIZE]; //use the stack to avoid fragmentation
	file_info_t *current_file = (file_info_t *)root_dir_buffer;

	for(size_t i = 0; i < drive_info->root_directory.size; i++) { //!!! I feel like there could be a bug in this
		readSector(drive_info->id, root_dir_buffer, drive_info->root_directory.start + i);

		for(size_t i = 0; i < 16; i++) { // 512 / 32 -> SECTOR_SIZE / sizeof(file_info_t) = 16
			switch((uint8_t)current_file[i].name[0]) { //special cases
				case 0x00: return NULL; //unused, there shouldn't be any more data
				case 0x2e: continue; //directory
				case 0xe5: continue; //deleted

				case 0x05: //char is actually 0xe5
					current_file[i].name[0] = 0xe5;
					break;
			}

			if(strncmp(filename, current_file[i].name, 11) == 0) {
				memcpy(file_info, &current_file[i], sizeof(file_info_t));
				return file_info;
			}
		}
	}

	return NULL;
}

static uint16_t _getFat12TableValue(uint8_t *buffer, const drive_info_t *drive_info, const uint16_t active_cluster) {
	//https://wiki.osdev.org/FAT#File_Allocation_Table
	uint16_t fat_index = active_cluster + (active_cluster/2); //must use ints
	uint16_t fat_sector = drive_info->fat.start + (fat_index / (drive_info->fat.size * SECTOR_SIZE));
	uint16_t table_value;

	readSector(drive_info->id, buffer, fat_sector);
	//printf("sector value: %d\r\n", fat_sector);

	table_value = *(uint16_t *)&buffer[fat_index];

	if(active_cluster & 0x0001) {
		table_value = table_value >> 4;
	} else {
		table_value = table_value & 0x0FFF;
	}

	//printf("table value: 0x%x\r\n", table_value);
	return table_value;
}

uint16_t copyFileContents(const drive_info_t *drive_info, const file_info_t *file_info, uint16_t output_segment) { //clusters being 512 bytes
	uint8_t fat_table[SECTOR_SIZE];
	uint16_t current_value = file_info->low_cluster_number;

	uint16_t current_cluster = file_info->low_cluster_number;
	size_t segment_sector = 0;

	do {
		readSectorFar(drive_info->id, output_segment, (segment_sector++ * SECTOR_SIZE),
			drive_info->data.start + current_value-2);
	} while((current_value = _getFat12TableValue(fat_table, drive_info, current_cluster++)) < 0xff7);

	if(current_value == 0xff7) {
		puts("Bad block in FAT");
		halt();
	}

	return output_segment;
}
