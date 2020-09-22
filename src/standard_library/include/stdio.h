#ifndef _STDIO_H
#define _STDIO_H

#include "io.h" //replace with kernel.h when syscalls are complete

void putchar(const char);
void puts(const char __far *);
void printf(const char __far *, ...);

char getchar();
char __far *gets(char __far *);

#endif
