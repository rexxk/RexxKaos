# Makefile for bootloader

AS=nasm

bootloader:
	$(AS) -f elf64 -o entry.o entry.asm

clean:
	rm -f entry.o
