#include "utils.h"

void halt() {
	while(1) asm ("hlt");
}

void panic() {
	puts("Kernel tried to exit, halting.");
	halt();
}
