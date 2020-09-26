#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

#include "io.h"
#include "stdlib.h" //itoa / NULL
#include "ctype.h" //isdigit
#include "string.h" //strlen

void putchar(const char);
void puts(const char __far *);
void printf(const char __far *, ...);

char getchar();
char __far *gets(char __far *);

#endif
