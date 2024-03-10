
        bits    32

        SECTION .text

        global _entry
_entry:

        jmp     _start



_start:
        mov     byte [driveNumber], dl

;        mov     edi, 0xB8000
;        mov     [edi], word 0x4545

        jmp     $



        SECTION .data

driveNumber:        db      0
bootMessage:        db      "Loading RexxKaos, a very simple operating system", 13, 10, 0
