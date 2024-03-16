        global _CopyMemory

; CopyMemory
_CopyMemory:
        mov     rcx, rdx
        rep     movsb
        push    rax
        pop     rax
        ret



        global _SetMemory

_SetMemory:
        mov     rax, rsi
        mov     rcx, rdx

        rep     stosb

        ret
