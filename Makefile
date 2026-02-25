.PHONY: all clean run

CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector

all: os-image.bin

# Compile kernel
kernel.bin: kernel/kernel_entry.o kernel/kernel.o kernel/idt.o kernel/keyboard.o kernel/isr.o
	$(LD) -o kernel.bin -Ttext 0x1000 kernel/kernel_entry.o kernel/kernel.o kernel/idt.o kernel/keyboard.o kernel/isr.o --oformat binary

kernel/kernel_entry.o: kernel/kernel_entry.asm
	nasm -f elf32 kernel/kernel_entry.asm -o kernel/kernel_entry.o

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel/kernel.o

kernel/idt.o: kernel/idt.c
	$(CC) $(CFLAGS) -c kernel/idt.c -o kernel/idt.o

kernel/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o kernel/keyboard.o

kernel/isr.o: kernel/isr.asm
	nasm -f elf32 kernel/isr.asm -o kernel/isr.o

# Compile bootloader
boot.bin: boot/boot.asm
	nasm -f bin boot/boot.asm -o boot.bin
	# Pad bootloader to exactly 512 bytes
	truncate -s 512 boot.bin

# Combine bootloader + kernel
os-image.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > os-image.bin
	# Pad to ensure proper disk size
	truncate -s 1474560 os-image.bin

run: os-image.bin
	qemu-system-x86_64 -drive format=raw,file=os-image.bin

clean:
	rm -f boot.bin kernel.bin os-image.bin kernel/*.o
