#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys.h"

#include "start.h"

extern program_args_t *start_data; //this should be accessible 1-1 as parameters below

void main(char *argv, uint8_t boot_drive) { //as all strings are stored sequentially, access like this- it may be easier to use start_data.argv as it has size info
	register uint16_t current_segment asm("ss");
	printf("current segment: %x\r\n", current_segment);
	printf("argv: %s\r\n", argv);
	printf("boot_drive: %x\r\n", boot_drive);

	char buffer[1024]; //1kb
	char filename_buffer[8+3+1];
	char *str;
	size_t command_len;

	/*
	if(segmentmap(start_data->boot_drive_id, "NOVEL   TXT", 0x1000)) {
		puts("Failed to read Novel.txt");
	} else {
		printf("Copied novel to 1000:0000\r\n");
		volatile char __far *c = (volatile char __far *)0x10000000;
		//putchar(*c);
		//for(;*c; c++) putchar(*c);
	}
	*/

	while(1) {
		memset(buffer, 0, sizeof(buffer)); //completely clear buffers
		strcpy(filename_buffer, "        COM");

		printf("> "); gets(buffer);

		for(str = buffer; *str != ' ' && *str; str++) *str = toupper(*str); //convert first arg to uppercase so that checking is easier

		//separate args
		str = strchr(buffer, ' ');
		if(str) { //NULL if no args
			*str++ = '\0'; //split line at args and point to the start of the arguments
		}
		command_len = strlen(buffer);

		if(command_len == 4 && strcmp(buffer, "EXIT") == 0) {
			return;

		} else if(command_len == 4 && strcmp(buffer, "ECHO") == 0) {
			if(str) puts(str); //check for NULL, "ECHO " is handled gracefully as ""
			else puts("");

			continue;
		}

		//copy filename to filename_buffer, so the extension is not required
		for(size_t i = 0; i < 8 && buffer[i]; i++) {
			filename_buffer[i] = buffer[i];
		}
		if(exec(boot_drive, filename_buffer, str)) printf("Unknown command: %s\r\n", buffer);
	}
}
