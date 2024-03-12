ENTRY(_entry)

OUTPUT_FORMAT(elf64-x86-64)

OUTPUT(kernel.sys)

SECTIONS {

    . = 0x1000000;

    .text : { *(.text) }

    .data : ALIGN(4096) { *(.data) }

    .bss : ALIGN(4096) { *(.bss) }
}
