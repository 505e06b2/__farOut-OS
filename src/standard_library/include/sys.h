#ifndef _SYS_H
#define _SYS_H

int __far segmentmap(uint8_t, const char *, uint16_t);
int __far exec(uint8_t, const char *, const char *);

#endif
