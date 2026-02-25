; Interrupt Service Routines

[BITS 32]

global irq1
extern keyboard_handler

; Keyboard interrupt handler (IRQ1)
irq1:
    pusha
    call keyboard_handler
    popa
    
    ; Send EOI to PIC
    mov al, 0x20
    out 0x20, al
    
    iret
