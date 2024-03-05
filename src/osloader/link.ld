ENTRY(_entry)

OUTPUT_FORMAT(elf64-x86-64)

OUTPUT(loader.bin)

SECTIONS {

    . = 0x20000;

    .strapcode : { *(.strapcode) }

    . = ALIGN(4096);

    .strapdata : { *(.strapdata) }

    . = 0x1000000;

    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
