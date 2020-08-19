bits 16
cpu  8086

mov ah, 0x09
mov al, 34
mov bh, 0
mov bl, 0xce
mov cx, 1
int 0x10
