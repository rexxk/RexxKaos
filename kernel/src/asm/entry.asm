
        bits    32

        SECTION .text


        global _entry
_entry:

        jmp     _start


        extern _kernel_entry

_start:
        mov     byte [driveNumber], dl

        call     _kernel_entry
        
;        mov     esi, 0xB8000
;        mov     [esi], word 0x4545

        jmp     $



        SECTION .data

driveNumber:        db      0
bootMessage:        db      "Loading RexxKaos, a very simple operating system", 13, 10, 0
