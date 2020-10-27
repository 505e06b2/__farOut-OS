#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

void main() {
	char buffer[1024]; //1kb
	char *str;
	size_t command_len;

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
