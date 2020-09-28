#ifndef _STDLIB_FARPTRS_C
#define _STDLIB_FARPTRS_C

void __far (*printf)(const char *, ...) = (void __far *)0x6000011f;
void __far (*puts)(const char *) = (void __far *)0x60000104;

#endif
