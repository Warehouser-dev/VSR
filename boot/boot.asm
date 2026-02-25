; VSR Bootloader - Loads kernel into memory and jumps to it
; This bootloader will be loaded by BIOS at address 0x7C00

[BITS 16]           ; We're in 16-bit real mode
[ORG 0x7C00]        ; BIOS loads bootloader here

KERNEL_OFFSET equ 0x1000  ; Load kernel at 4KB

start:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00  ; Stack grows downward from bootloader

    ; Print boot message
    mov si, msg_boot
    call print_string

    ; Load kernel from disk
    call load_kernel

    ; Print loading message
    mov si, msg_loaded
    call print_string

    ; Switch to protected mode
    call switch_to_pm

    ; Should never reach here
    cli
    hlt

; Function to print a null-terminated string
print_string:
    mov ah, 0x0E    ; BIOS teletype function
.loop:
    lodsb           ; Load byte from SI into AL, increment SI
    cmp al, 0       ; Check for null terminator
    je .done
    int 0x10        ; BIOS video interrupt
    jmp .loop
.done:
    ret

; Load kernel from disk
load_kernel:
    mov bx, KERNEL_OFFSET  ; Load kernel to 0x1000
    mov dh, 15             ; Read 15 sectors (enough for small kernel)
    mov dl, 0x80           ; Boot drive
    call disk_load
    ret

; Read sectors from disk
; DH = number of sectors, DL = drive, BX = destination
disk_load:
    push dx
    
    mov ah, 0x02        ; BIOS read sector function
    mov al, dh          ; Number of sectors to read
    mov ch, 0           ; Cylinder 0
    mov dh, 0           ; Head 0
    mov cl, 2           ; Start from sector 2 (sector 1 is bootloader)
    
    int 0x13            ; BIOS disk interrupt
    jc disk_error       ; Jump if error (carry flag set)
    
    pop dx
    ret

disk_error:
    mov si, msg_disk_error
    call print_string
    cli
    hlt

; Switch to 32-bit protected mode
switch_to_pm:
    cli                 ; Disable interrupts
    lgdt [gdt_descriptor] ; Load GDT
    
    mov eax, cr0
    or eax, 1           ; Set PE (Protection Enable) bit
    mov cr0, eax
    
    jmp 0x08:init_pm    ; Far jump to 32-bit code

[BITS 32]
init_pm:
    ; Set up segment registers for protected mode
    mov ax, 0x10        ; Data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    mov esp, 0x90000    ; Set up stack
    
    ; Jump to kernel
    call KERNEL_OFFSET
    
    ; If kernel returns, halt
    cli
    hlt

[BITS 16]

; GDT (Global Descriptor Table)
gdt_start:
    ; Null descriptor
    dq 0

gdt_code:
    ; Code segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0            ; Base (bits 0-15)
    db 0            ; Base (bits 16-23)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0            ; Base (bits 24-31)

gdt_data:
    ; Data segment descriptor
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size
    dd gdt_start                 ; Address

; Data
msg_boot db 'VSR Bootloader v0.1', 0x0D, 0x0A, 0
msg_loaded db 'Kernel loaded. Entering protected mode...', 0x0D, 0x0A, 0
msg_disk_error db 'Disk read error!', 0x0D, 0x0A, 0

; Boot signature
times 510-($-$$) db 0   ; Pad with zeros
dw 0xAA55               ; Boot signature (must be at bytes 510-511)
