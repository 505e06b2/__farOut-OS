#include "fs.h"

bpb_t *getBPB(uint8_t drive_id, uint8_t *buffer) {
	bpb_t *bpb = (bpb_t *)buffer;
	readSector(drive_id, buffer, 0);

	if(bpb->signature[0] == 0x55 && bpb->signature[1] == 0xaa) {
		return bpb;
	} else {
		return NULL;
	}
}

void print_directory(file_info_t *current_file) {
	char text_buffer[30];
	for(; ((uint8_t *)current_file)[0] != 0; current_file++) { //does the sizeof for me
		if(current_file->attributes == LONG_FILENAME) continue;
		if(current_file->attributes == VOLUME_ID) continue;
		print("= ");
		for(char *str = current_file->name; str < current_file->name + 8; str++) putchar(*str);
		putchar('.');
		for(char *str = current_file->name+8; str < current_file->name + 8+3; str++) putchar(*str);
		print("\r\n    ");
		print("Attributes    => 0x");
			print(itoa(current_file->attributes, text_buffer, 16));
			print("\r\n    ");
		print("Size (bytes)  => ");
			print(itoa(current_file->size, text_buffer, 10));
			print("\r\n    ");
		print("Created       => (UTC) ");
			print(itoa(current_file->creation_time.value.hours, text_buffer, 10));
			putchar(':');
			print(itoa(current_file->creation_time.value.minutes, text_buffer, 10));
			putchar(':');
			print(itoa(current_file->creation_time.value.seconds*2, text_buffer, 10));
		putchar(' ');
			print(itoa(current_file->creation_date.value.day, text_buffer, 10));
			putchar('/');
			print(itoa(current_file->creation_date.value.month, text_buffer, 10));
			putchar('/');
			print(itoa(current_file->creation_date.value.year + 1980, text_buffer, 10));
			print("\r\n    ");
		print("Modified      => (UTC) ");
			print(itoa(current_file->modified_time.value.hours, text_buffer, 10));
			putchar(':');
			print(itoa(current_file->modified_time.value.minutes, text_buffer, 10));
			putchar(':');
			print(itoa(current_file->modified_time.value.seconds*2, text_buffer, 10));
		putchar(' ');
			print(itoa(current_file->modified_date.value.day, text_buffer, 10));
			putchar('/');
			print(itoa(current_file->modified_date.value.month, text_buffer, 10));
			putchar('/');
			print(itoa(current_file->modified_date.value.year + 1980, text_buffer, 10));
		print("\r\n");
	}
}
