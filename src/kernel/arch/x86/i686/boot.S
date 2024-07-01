	/* A bridge between the GRUB bootloader and the kernel.
	 *
	 * This source file sets up important header symbols so that the bootloader
	 * can detect this custom kernel. Additionally, it sets up a 2MiB stack
	 * before jumping to the _kmain function in the kernel.
	 */

	/* Declare constants for the Multiboot header.
	 */
	.set MAGIC, 0x1badb002			# Magic number for GRUB bootloader
	.set ALIGN, 1<<0				# Align loaded modules on page boundaries
	.set MEMINFO, 1<<1				# Provide memory map
	.set FLAGS, (ALIGN | MEMINFO)
	.set CHECKSUM, -(MAGIC + FLAGS)

	/* Declare Multiboot header that marks the program as a kernel.
	 *
	 * These are magic values documented in the Multiboot Specification. The
	 * bootloader will search for this signature in the first 8 KiB of the
	 * kernel file, aligned at a 32-bit boundary. The signature is in its own
	 * section so the header can be forced to be within the first 8 KiB of the
	 * kernel file.
	 */
	.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

	/* The linker script specifies _start as the entry point to the kernel. The
	 * bootloader will jump to this position once the kernel has been loaded.
	 */
	.section .text
	.global _start
	.type _start, @function
_start:
	/* To set up a stack, we set the ESP register to point to the top of stack
	 * (as it grows downwards on x86 systems). This is necessarily done in ASM
	 * as languages like C cannot function without a stack.
	 */
	mov $stack, %esp
	/* C++ requires runtime support for global constructors and exceptions. To
	 * provide this, we have defined a function in kernel.cpp to set up
	 * rudimentary support for constructors. We have no interest in providing
	 * support for exceptions.
	 */
	.extern call_ctors
	call call_ctors
	/* Push two values to the stack, to be available as parameters for the
	 * kernel main function.
	 *
	 * According to the x86 C/C++ calling conventions, function arguments are
	 * pushed on the stack from right to left, such that the last push before
	 * call represents the first argument. [1][2]
	 *
	 * [1]: https://stackoverflow.com/a/9141701/10052039
	 * [2]: https://en.wikipedia.org/wiki/X86_calling_conventions#List_of_x86_calling_conventions
	 */
	push %eax                       # Multiboot magic number: 0x2badb002
	push %ebx                       # Physical ptr to Multiboot info structure
	/* Enter the high-level kernel.
	 *
	 * NOTE: OSDev.org mentions that the System V ABI requires the stack to be
	 * 16-byte aligned at the time of the call instruction. Since the code
	 * example in the guide did not push any bytes to the stack, the alignment
	 * was preserved and the call was well-defined. However, we just pushed two
	 * 32-bit registers to the stack -- does this mean we did not preserve the
	 * alignment?
	 */
	.extern _kmain
	call _kmain
	/* If the system has nothing more to do, put the computer into an infinite
	 * loop. To do that:
	 *
	 * 1) Disable interrupts with cli (clear interrupts).
	 * 2) Wait for the next interrupt to arrive with hlt.
	 * 3) Jump to the hlt instruction if it ever wakes up due to a non-maskable
	 *    interrupt occurring or due to system management mode.
	 */
	cli
1:	hlt
	jmp 1b

	.section .bss
	.space 2*1024*1024				# Add 2MiB padding
stack:								# Will point to some available space in RAM
