#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "sys.h"

#include "start.h"

extern program_args_t *start_data; //this should be accessible 1-1 as parameters below

void main(char *argv, uint8_t boot_drive) { //as all strings are stored sequentially, access like this- it may be easier to use start_data.argv as it has size info
	printf("argv: %s\r\n", argv);
	printf("boot_drive: %x\r\n", boot_drive);

	char buffer[1024]; //1kb
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
		//memset(buffer, 0, sizeof(buffer)); //completely clear buffer - just check properly instead
		printf("> "); gets(buffer);

		for(str = buffer; *str != ' ' && *str; str++) *str = toupper(*str); //convert first arg to uppercase so that checking is easier

		//separate args
		str = strchr(buffer, ' ');
		if(str) { //NULL if no args
			*str++ = '\0'; //split line at args and point to start
		}
		command_len = strlen(buffer);

		if( (strcmp(buffer, "EXIT") == 0) && (command_len == 4) ) {
			return;

		} else if( (strcmp(buffer, "ECHO") == 0) && (command_len == 4) ) {
			if(str) puts(str); //check for NULL, "ECHO " is handled gracefully as ""
			else puts("");

			continue;
		}

		printf("Unknown command: %s\r\n", buffer);
	}
}
