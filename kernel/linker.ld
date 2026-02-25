/* Linker script for VSR kernel */

ENTRY(_start)

SECTIONS
{
    . = 0x1000;  /* Kernel loads at 1MB */
    
    .text : {
        *(.text)
    }
    
    .data : {
        *(.data)
    }
    
    .bss : {
        *(.bss)
    }
}
