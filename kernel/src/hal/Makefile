# Makefile for bootloader

AS=nasm

BUILD_DIR= build
ASFLAGS= -f elf64
ASMSOURCES = $(shell find -name '*.asm')
ASMOBJECTS = $(patsubst %.asm, %.o, $(ASMSOURCES))

$(ASMOBJECTS): %.o : $(ASMSOURCES)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f *.o
