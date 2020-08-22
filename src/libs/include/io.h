#ifndef IO_H
#define IO_H

void putchar(char);
void print(char *);
void puts(char *);

void screen_clear();

char getchar();

char *readSector(unsigned char, unsigned char *, unsigned char);

#endif