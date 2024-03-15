        bits 64

        SECTION .text

        ; _inport - get data from address
        ; Parameters:
        ;   uint16_t address ( DI )
        ;
        ; Return value:
        ;   uint8_t value ( AL )

        global  _inport

_inport:

        push    dx

        mov     dx, di
        in      al, dx

        pop     dx

        ret



        ; _outport - set data to address
        ; Parameters:
        ;   uint16_t address ( DI )
        ;   uint8_t value ( SI )

        ; Return value:
        ;   none

        global _outport

_outport:

        push    dx

        mov     dx, di
        mov     ax, si
        out     dx, al

        pop     dx

        ret
