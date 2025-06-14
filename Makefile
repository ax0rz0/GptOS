CC = gcc
CFLAGS = -ffreestanding -m32 -nostdlib -fno-builtin -I.
LDFLAGS = -T linker.ld -nostdlib

all: gptos.img

bootloader.bin: bootloader.asm
	nasm -f bin $< -o $@

kernel.o: kernel.c keyboard.c apps/calc.c apps/notes.c apps/extras.c
	$(CC) $(CFLAGS) -c kernel.c keyboard.c apps/calc.c apps/notes.c apps/extras.c

kernel.bin: kernel.o
	ld $(LDFLAGS) -o kernel.bin kernel.o

gptos.img: bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > gptos.img

run: gptos.img
	qemu-system-i386 -drive format=raw,file=gptos.img

usb: gptos.img
	@echo "Insert USB, identify device (eg. /dev/sdX), then run:"
	@echo "sudo dd if=gptos.img of=/dev/sdX bs=512 seek=0"

clean:
	rm -f *.o *.bin gptos.img