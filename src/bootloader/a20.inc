

EnableA20:

        cli

        call    A20Wait
        mov     al, 0xAD
        out     0x64, al

        call    A20Wait
        mov     al, 0xD0
        out     0x64, al

        call    A20Wait2
        in      al, 0x60
        push    eax

        call    A20Wait
        mov     al, 0xD1
        out     0x64, al

        call    A20Wait
        pop     eax
        or      al, 2
        out     0x60, al

        call    A20Wait
        mov     al, 0xAE
        out     0x64, al

        call    A20Wait

        sti

        ret

A20Wait:
        in      al, 0x64
        test    al, 2
        jnz     A20Wait
        ret

A20Wait2:
        in      al, 0x64
        test    al, 1
        jz      A20Wait2
        ret
