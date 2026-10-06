ENTRY(\_start)
This is like telling the Linker, "When you build the final file, stamp a note on the cover saying the absolute starting point is the \_start label." This connects perfectly to the .global \_start you wrote in your assembly file. When GRUB loads your OS, it reads this stamp to know exactly where to point the CPU first.

SECTIONS {
This opens the main blueprint. Everything inside these brackets tells the Linker exactly how to arrange the different chunks (sections) of memory in the final OS file.

    . = 1M;
    The dot . is called the Location Counter. It represents the current memory address. By setting it to 1M (1 Megabyte), you are telling the Linker, "Pretend the very first byte of this file is going to be loaded at the 1 Megabyte mark in the computer's RAM."
    Why 1MB? On x86 computers, the first 1 Megabyte of RAM (from 0 to 1,048,575) is a chaotic minefield. It's reserved for the motherboard's BIOS (Basic Input/Output System), legacy floppy disk controllers, and video memory. If you put your OS there, it will crash instantly. The 1MB mark is the universally agreed-upon "safe zone" where x86 kernels begin.

    .text : { *(.multiboot) *(.text) }
    The * acts like a wildcard (meaning "grab this from ALL files").
    You are telling the Linker: "Create a .text section. First, grab the .multiboot section from my assembly file (the magic numbers!) and glue it at the very front. Immediately after that, glue all the .text sections (the executable code) from my Assembly and C files."
    Because of this line, your magic number 0x1BADB002 is guaranteed to be sitting at exactly 1MB in RAM, making it instantly visible to the GRUB bootloader.

    .rodata : { *(.rodata*) }
    rodata stands for Read-Only Data. When you write printf("Hello World"); in your C code, the actual text string "Hello World" gets saved here. By putting it in its own section, the OS can lock this part of memory later so no rogue program can accidentally overwrite your text strings.

    .data : { *(.data) }
    This section grabs all initialized variables from your C code. If you write int player_health = 100; globally in C, the number 100 gets physically saved into the hard drive file in this section, and loaded into RAM right here.

    .bss : { *(COMMON) *(.bss) }
    This section grabs all uninitialized variables and empty space. If you write int empty_array[5000]; in C, or create a 16KB stack in assembly (.skip 16384), it goes here. The Linker notes how much total empty space is needed, but it doesn't actually write gigabytes of zeroes to your file. It just tells the bootloader, "When you load us, clear out this much extra RAM at the end."

}

## Core OS Concepts

# The Kernel vs. User Programs

When the computer turns on, it only loads exactly one program into RAM: your OS. Once your OS boots up and kernel_main starts running, the GRUB bootloader's job is permanently finished. Your OS is now the undisputed boss of the hardware.
If you want to run a separate program later (like a calculator app or a game):
The bootloader doesn't load it; your OS does. Your C code reaches out to the hard drive, reads the calculator file, and copies it into RAM.
It goes somewhere else. Your OS looks at the RAM map and says, "1MB is already taken by me. But 8MB is empty. I'll load the calculator at the 8MB mark."
It uses a different magic number. User programs don't use the Multiboot magic number because they aren't booting the computer. They use file formats like ELF (Linux) or PE / .exe (Windows). Those files have their own, different magic numbers at the top so your OS knows how to run them.

# Why Read-Only? Why not Read/Write?

Imagine you write this in your OS: printf("System Booting...");

If the text "System Booting..." was saved in normal Read/Write memory (the .data section), any rogue bug in your OS could accidentally overwrite it. If a bad pointer in your C code accidentally overwrote the first letter with an 'X', every time you restarted your computer, it would print "Xystem Booting...".

By putting hardcoded strings into .rodata (Read-Only Data), you are telling the CPU's hardware memory controller to lock the door. If a virus or a bug in your C code tries to alter that memory address, the CPU physically blocks the electrical signal and throws a fatal hardware error (a Segmentation Fault).

It is a massive safety net. Variables that are meant to change (int score = 0;) go to .data (Read/Write). Things that should never change in the middle of the program running (like hardcoded text strings or math constants like Pi) go to .rodata.

# Why isn't printf in .rodata?

In computer architecture, there is a massive physical difference between Code (Actions) and Data (Nouns).
printf is Code: It is a function made up of active machine instructions (mov, add, call, jmp). The CPU actually executes it. Therefore, printf goes into the .text section.
"Hello World" is Data: The CPU cannot "execute" the letter H. It is just a static piece of information waiting to be read. Therefore, it goes into a data section.
