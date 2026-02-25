# VSR Operating System - Phase 1: Bootloader

A minimal x86 bootloader for the VSR OS that runs in QEMU. Built with claude 4.5 as an experiment to test and see how good AI does with making an OS.
<img width="717" height="396" alt="Screenshot 2026-02-25 at 21 43 13" src="https://github.com/user-attachments/assets/33f35b03-fe46-471d-8b0e-93e7a676bed4" />
## Prerequisites

Install the required tools:

```bash
# macOS
brew install nasm qemu
```

## Build and Run

```bash
# Build the bootloader
make

# Run in QEMU
make run


# Clean build artifacts
make clean
```

## What's Happening

1. BIOS loads our 512-byte bootloader at memory address 0x7C00
2. Bootloader sets up segment registers and stack
3. Prints a welcome message using BIOS interrupts
4. Halts the CPU

## Next Steps

- Add keyboard input handling
- Load a second stage bootloader or kernel
- Switch to protected mode (32-bit)
- Set up basic memory management

## File Structure

- `boot/boot.asm` - Bootloader source code (NASM assembly)
- `Makefile` - Build automation
- `boot.bin` - Compiled bootloader binary (512 bytes)
# VSR
