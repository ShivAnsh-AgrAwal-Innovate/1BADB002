boot.s
Kernel entry point. It declares the Multiboot header, sets up a stack, then calls kernel_main.

.set MAGIC, 0x1BADB002
The standard required signature for a bootable OS kernel is 0x1BADB002. If you look closely at the hex, it's programmer humor: it spells "1 BAD B002" (One Bad Boot). The word MAGIC in the code is just the variable name we chose.

.set FLAGS, 0
The bootloader can do a few favors for your OS before it hands over control. (like a memory map or video mode setup). You request these favors using the FLAGS variable. 

.set CHECKSUM, -(MAGIC + FLAGS)
Imagine you have an MP3 or a JPEG on your hard drive, and by pure, random chance, a cluster of bytes inside that image file matches 0x1BADB002. The bootloader might try to run your picture of a dog as an Operating System, instantly crashing the computer. To prevent this, the creators of the Multiboot standard added a Checksum. 
The rule is: Magic + Flags + Checksum MUST equal 0.

# What are Sections? 
When you compile a C program, the compiler doesn't just mash everything together into a giant blob. It sorts your program into distinct "rooms" or Sections so the computer knows how to handle them.

.section .multiboot
Holds the magic passwords (like 0x1BADB002) so the bootloader recognizes your file as a real OS. It gets forced to the very front of your compiled file so GRUB(Grand Unified Bootloader) finds it instantly.

.align 4
A 32-bit CPU reads memory in 4-byte chunks. Because of this, it expects 4-byte numbers to start at a memory address that is a multiple of 4 (like address 0, 4, 8, 12, 16...). If a number starts at a weird address (like address 3), the CPU has to do two separate memory reads and stitch them together, which slows everything down. On some CPU architectures, reading misaligned memory just instantly crashes the processor.
.align 4 tells the assembler: "If our current spot in the file isn't on an address divisible by 4, pad it with blank bytes until it is." This guarantees the bootloader finds your data on the exact grid it expects.

# What is .long?
In C, when you want to store a 32-bit integer, you declare it using int or uint32_t. In GNU Assembly, you use data directives to tell the assembler how much space to carve out:
.byte reserves 1 byte (like C's char)
.word reserves 2 bytes (like C's short)
.long reserves 4 bytes (32 bits)

.long MAGIC
.long FLAGS
.long CHECKSUM

Initially we didn't actually put any code or data into the OS file. It just acted like a #define in C. The long lines are what physically turn the raw data into your compiled binary.

.section .bss
Tells the system to reserve a block of empty, blank RAM when the OS boots without taking up space on your hard drive. Here we use a space of 16KB

.align 16
Just like .align 4 put our magic numbers on a 4-byte grid, this forces the start of our stack onto a 16-byte grid in memory. Why 16? Because the official rulebook for C compilers (the System V ABI) demands it. If you don't do this, the moment your code in assembly jumps into your C code, the C compiler might try to run advanced math instructions (like SSE/SIMD) that will instantly crash your CPU if the memory isn't 16-byte aligned.

stack_bottom:
This isn't an instruction; it’s just a label. It acts exactly like a pointer in C. It tags the current memory address so we have a name for the absolute lowest point of our stack.

.skip 16384
This tells the assembler to advance 16,384 bytes (exactly 16 Kilobytes) forward, leaving it entirely blank. Because this is happening inside the .bss section, it doesn't add 16KB of zeroes to your hard drive file; it just tells the GRUB bootloader, "Hey, make sure you reserve 16KB of empty RAM right here when you load me."

stack_top:
This is another label tagging the memory address exactly 16KB after stack_bottom.

.section .text
This is the read-only vault where your actual executable machine code (the mov, call, and hlt instructions) lives and runs.

.global _start
In C, when you write int main(), the compiler automatically knows that's the starting point of your app. In raw assembly, there are no assumptions. You have to explicitly tell the Linker program, "Hey, this label called _start is the absolute front door to the entire operating system." .global makes the label visible to other files.

_start:
This is the actual front door. When the GRUB bootloader finishes loading your OS into RAM, this is the exact memory address where it points the CPU and says, "Go."

mov $stack_top, %esp
mov means move. %esp is a tiny, super-fast memory slot directly inside the CPU hardware called the Extended Stack Pointer. This line takes the memory address of stack_top (the 16KB of empty space we carved out earlier) and jams it into that CPU register. Congratulations! Your OS now has a working stack.

call kernel_main
This is the baton pass! The call instruction tells the CPU to jump over to your C file and run the function named kernel_main. Because of the previous line, when C starts creating variables, it knows exactly where to put them. From this exact millisecond forward, your C code is driving the computer.

cli
Stands for Clear Interrupts. In a perfect OS, your C kernel_main function has a massive while(1) loop inside it and never, ever finishes. But if your C code does accidentally finish, crash, or return, control drops back into this assembly code. cli flips a physical hardware switch inside the CPU that tells it to go deaf. It blocks all signals from the keyboard, mouse, and timer so they can't interrupt the CPU's final moments.

1: hlt
hlt stands for Halt. It shuts down the CPU's processing engine, putting it into a deep, low-power sleep state. Normally, a CPU sleeps here until you press a key on the keyboard to wake it up. But because we just ran cli, the CPU is deaf to the keyboard. It is trapped in a coma. The 1: is just a local, temporary label. You will get it why we need this after reading what jmp 1b does.

jmp 1b
jmp means jump. 1b means "jump backward to the nearest label named 1". This is a paranoid failsafe. There is a rare type of hardware emergency called a Non-Maskable Interrupt (NMI) like your RAM literally catching fire or failing (a catastrophic, physical hardware malfunction where the computer can no longer trust its own electrical signals.) that can bypass cli and violently wake the CPU up. If that happens, the CPU wakes up, hits this jump command, loops backward to 1:, hits hlt, and goes instantly back to sleep before it can cause any more damage.
