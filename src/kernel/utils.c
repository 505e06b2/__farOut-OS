#include "utils.h"

void halt() {
	while(1) asm volatile (
		"cli;"
		"hlt;"
	);
}

void panic() {
	printString("Kernel tried to exit, halting."); //if data_segment not set to 0x7000, this will not show
	halt();
}
