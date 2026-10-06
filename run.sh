as --32 boot.s -o boot.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel.c -o kernel.o
ld -m elf_i386 -T linker.ld boot.o kernel.o -o kernel.bin

qemu-system-i386 -kernel kernel.bin
