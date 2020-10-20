#include "stdlib_farptrs.c"

void main() {
	char buffer[1024]; //1kb
	char *str;

	while(1) {
		//memset(buffer, 0, sizeof(buffer)); //completely clear buffer - just check properly instead
		printf("> "); gets(buffer);

		for(str = buffer; *str != ' ' && *str; str++) *str = toupper(*str); //convert first arg to uppercase so that checking is easier

		//no args
		if(strcmp(buffer, "EXIT") == 0 && strlen(buffer) == 4) {
			return;
		}

		//>= 0 args
		str = strchr(buffer, ' ');
		if(str) { //NULL if no args
			*str++ = '\0'; //split line at args and point to start
		}

		if( (strcmp(buffer, "ECHO") == 0) && strlen(buffer) == 4 ) {
			if(str) puts(str); //check for NULL, "ECHO " is handled gracefully as ""
			else puts("");

			continue;
		}

		printf("Unknown command: %s\r\n", buffer);
	}
}
