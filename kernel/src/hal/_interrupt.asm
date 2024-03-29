        bits 64

        global _EnableInterrupts

_EnableInterrupts:
        sti
        ret



        global _DisableInterrupts

_DisableInterrupts:
        cli
        ret



        global _SetInterruptDescriptorTable

_SetInterruptDescriptorTable:
        lidt    [rdi]
        ret
