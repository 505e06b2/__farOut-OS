#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include "stdint.h"

#include "stdlib.h" //itoa
#include "ctype.h" //isdigit
#include "string.h" //strlen

void __far putchar(const char);
void __far puts(const char *);
void __far printf(const char *, ...);

char __far getchar();
char * __far gets(char *);

#endif
