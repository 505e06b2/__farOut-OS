void main() {
	asm volatile (
		"mov ah, 0x0e;"  //teletype print
		"mov al, 37;"
		"xor bx, bx;"     //write to 0th page
		"int 0x10;"
		:
		:
		: "ax", "bx"
	);
}
