#include "fs.h"

bpb_t *getBPB(uint8_t drive_id, bpb_t *bpb) {
	readSector(drive_id, (uint8_t *)bpb, 0);

	if(bpb->signature[0] == 0x55 && bpb->signature[1] == 0xaa) {
		return bpb;
	} else {
		return NULL;
	}
}

file_info_t *find_file_info(uint8_t drive_id, bpb_t *bpb_info, file_info_t *file_info, const char *filename) {
	const uint16_t root_dir_start = bpb_info->reserved_sectors + (bpb_info->fats * bpb_info->sectors_per_fat);
	const uint16_t root_dir_sectors = ((bpb_info->root_directory_entries * sizeof(file_info_t)) + (bpb_info->bytes_per_sector - 1)) / bpb_info->bytes_per_sector; //round to the biggest sector

	//use the stack to avoid fragmentation
	uint8_t root_dir_buffer[bpb_info->bytes_per_sector * root_dir_sectors]; //this could be optimised for sector *2, but only after printf
	file_info_t *current_file = (file_info_t *)root_dir_buffer;

	for(size_t i = 0; i < root_dir_sectors; i++) { //!!! I feel like there could be a bug in this
		readSector(drive_id, root_dir_buffer + (i * bpb_info->bytes_per_sector), root_dir_start + i);
	}

	for(; ((uint8_t *)current_file)[0] != 0; current_file++) { //does the pointer arithmetic for me
		if(strncmp(filename, current_file->name, 11) == 0) {
			memcpy(file_info, current_file, sizeof(file_info_t));
			return file_info;
		}
	}
	return NULL;
}

#ifdef print_root_directory
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
#endif
