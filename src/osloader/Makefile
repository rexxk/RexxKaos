# Makefile for bootloader

AS=nasm

bootloader:
	$(AS) -f elf64 -o osloader.o osloader.asm

clean:
	rm -f osloader.o
