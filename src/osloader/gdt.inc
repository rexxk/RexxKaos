
        SECTION .strapcode

LoadGDT:

        lgdt    [gdtEntry]

        ret
