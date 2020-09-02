#include "fs.h"

bpb_t *getBPB(uint8_t drive_id, bpb_t *bpb_info) {
	uint8_t bootsector[SECTOR_SIZE];
	readSector(drive_id, bootsector, 0);

	if(bootsector[SECTOR_SIZE-2] == 0x55 && bootsector[SECTOR_SIZE-1] == 0xaa) {
		memcpy(bpb_info, bootsector+3, sizeof(bpb_t)); //+3 to remove the jmp
		return bpb_info;
	} else {
		return NULL;
	}
}

file_info_t *findFileInfo(uint8_t drive_id, bpb_t *bpb_info, file_info_t *file_info, const char *filename) {
	const uint16_t root_dir_start = bpb_info->reserved_sectors + (bpb_info->fats * bpb_info->sectors_per_fat);
	const uint16_t root_dir_sectors = ((bpb_info->root_directory_entries * sizeof(file_info_t)) + (SECTOR_SIZE - 1)) / SECTOR_SIZE; //round to the biggest sector

	//use the stack to avoid fragmentation
	uint8_t root_dir_buffer[SECTOR_SIZE];
	file_info_t *current_file = (file_info_t *)root_dir_buffer;

	for(size_t i = 0; i < root_dir_sectors; i++) { //!!! I feel like there could be a bug in this
		readSector(drive_id, root_dir_buffer, root_dir_start + i);

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

uint16_t copyFileContents(uint8_t drive_id, uint16_t segment, uint16_t start_cluster) { //this would need to be int32 if fat32+
	readSectorFar(drive_id, segment, 0, 0);
	return segment;
}

/*!!! disable as it's huge + now out of date
	void print_root_directory(uint8_t drive_id, const bpb_t *bpb_info) {
		const uint16_t root_dir_start = bpb_info->reserved_sectors + (bpb_info->fats * bpb_info->sectors_per_fat);
		const uint16_t root_dir_sectors = ((bpb_info->root_directory_entries * sizeof(file_info_t)) + (bpb_info->bytes_per_sector - 1)) / bpb_info->bytes_per_sector; //round to the biggest sector

		//use the stack to avoid fragmentation
		uint8_t root_dir_buffer[bpb_info->bytes_per_sector * root_dir_sectors]; //this could be optimised for sector *2, but only after printf
		file_info_t *current_file = (file_info_t *)root_dir_buffer;

		for(size_t i = 0; i < root_dir_sectors; i++) { //!!! I feel like there could be a bug in this
			readSector(drive_id, root_dir_buffer + (i * bpb_info->bytes_per_sector), root_dir_start + i);
		}

		puts("Root Directory:");
		for(; ((uint8_t *)current_file)[0] != 0; current_file++) { //does the sizeof for me
			if(((uint8_t *)current_file)[0] == 0xe5) continue; //unused
			if(current_file->attributes == LONG_FILENAME) continue;
			if(current_file->attributes == VOLUME_ID) continue;
			//filename
			printf("= %8s.%3s           \r\n    ", current_file->name, current_file->name+8);  //need the padding to replace the "press to continue text"
			printf("Attributes      => 0x%2x\r\n    ", current_file->attributes);
			printf("Size    (bytes) => %d\r\n    ", current_file->size);
			printf("Created   (UTC) => %2d:%2d:%2d %2d/%2d/%4d\r\n    ",
				current_file->creation_time.value.hours,
				current_file->creation_time.value.minutes,
				current_file->creation_time.value.seconds*2,

				current_file->creation_date.value.day,
				current_file->creation_date.value.month,
				current_file->creation_date.value.year + 1980
			);

			printf("Modified  (UTC) => %2d:%2d:%2d %2d/%2d/%4d\r\n    ",
				current_file->modified_time.value.hours,
				current_file->modified_time.value.minutes,
				current_file->modified_time.value.seconds*2,

				current_file->modified_date.value.day,
				current_file->modified_date.value.month,
				current_file->modified_date.value.year + 1980
			);

			print("\r\n");
			print("Press any key to continue\r");
			getchar();
		}
	}
*/
