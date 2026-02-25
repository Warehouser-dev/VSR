; Kernel Entry Point
; This is the first code that runs when the kernel is loaded

[BITS 32]           ; We're in 32-bit protected mode now
[EXTERN kernel_main] ; Our C kernel main function

global _start

_start:
    ; Set up segment registers for kernel
    mov ax, 0x10    ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov esp, 0x90000
    
    ; Call C kernel
    call kernel_main
    
    ; If kernel returns, halt
    cli
    hlt
