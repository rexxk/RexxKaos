
        bits    16

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



        jmp     $



driveNumber:        db      0



        SECTION .strapdata

