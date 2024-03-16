
        bits    64

        SECTION .text


        global _entry
_entry:
        pop	rdx
        pop	rcx
        pop	rbx

        jmp     _start


        extern _kernel_entry

_start:
        mov     byte [driveNumber], dl
        mov     qword [bootDataAddress], rcx

        call     _kernel_entry
        
;        mov     esi, 0xB8000
;        mov     [esi], word 0x4545

        sti

        jmp     $



        SECTION .data

driveNumber:        db      0
bootMessage:        db      "Loading RexxKaos, a very simple operating system", 13, 10, 0

bootDataAddress:        dq      0
