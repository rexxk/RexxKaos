
        bits    16

        default rel

        SECTION .strapcode

        global _entry
_entry:
;        mov     byte [driveNumber], dl

        mov     ax, 0x2000
        mov     ds, ax
        mov     es, ax

        mov     ax, 0x1000
        mov     ss, ax
        mov     sp, 0xFFFF

        mov     esi, bootMessage
        call    PrintString


        jmp     $



; PrintString
;  - Read character from ds:si and print if it is not null termination

PrintString:

		lodsb
		or		al, al
		jz		.printDone

		mov		ah, 0xE
		int		0x10
		jmp		PrintString

.printDone:
		ret




        SECTION .strapdata

driveNumber:        db      0
bootMessage:        db      "Loading RexxKaos, a very simple operating system", 13, 10, 0
