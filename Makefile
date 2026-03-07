CC = gcc
AS = as
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector
ASFLAGS = --32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Final Product
casino.bin: boot.o kernel.o
	ld $(LDFLAGS) -o casino.bin boot.o kernel.o

# Assembly Object
boot.o: boot.s
	$(AS) $(ASFLAGS) boot.s -o boot.o

# C Object
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

# Emulator Target
run: casino.bin 
	qemu-system-i386 -kernel casino.bin

# Cleaup Utility
clean: 
	rm -f *.o *.bin