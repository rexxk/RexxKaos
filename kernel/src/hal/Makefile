# Makefile for bootloader

AS=nasm

BUILD_DIR= build
ASFLAGS= -f elf64
ASMSOURCES = $(wildcard *.asm)
ASMOBJECTS = $(patsubst %.asm, %.o, $(ASMSOURCES))

all: $(ASMOBJECTS)

%.o: %.asm
	$(AS) $(ASFLAGS) $<

clean:
	rm -f *.o
