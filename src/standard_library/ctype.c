#include "ctype.h"

int __far isdigit(int c) {
	if(c >= 48 && c <= 57) return 1;
	return 0;
}

int internal_isdigit(int c) {
	return isdigit(c);
}

int __far isalpha(int c) {
	if( (c >= 97 && c <= 122) || (c >= 65 && c <= 90) ) return 1; //lowercase || uppercase
	return 0;
}

int __far toupper(int c) {
	if(c >= 97 && c <= 122) return c-32;
	return c;
}
