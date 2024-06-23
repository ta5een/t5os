/**
 * This source file functions as a bridge between the GRUB bootloader and the
 * kernel. It sets up important header symbols so that the bootloader can
 * detect this custom kernel. Additionally, it sets up a 2MiB stack before
 * jumping to the `_kmain` function in the kernel.
 */

.set MAGIC, 0x1badb002          /* Magic number for GRUB bootloader */
.set ALIGN, 1<<0                /* Align loaded modules on page boundaries */
.set MEMINFO, 1<<1              /* Provide memory map */
.set FLAGS, (ALIGN | MEMINFO)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
.extern _kmain                  /* Entry point of kernel */
.extern call_ctors              /* Support for C++ constructors */

.global loader                  /* Entry point for the linker */
loader:
  mov $stack, %esp              /* Set up the kernel stack */
  call call_ctors               /* Invoke C++ function */
  push %eax                     /* Push `AX` register (multiboot) to stack */
  push %ebx                     /* Push `BX` register (magic number) to stack */
  call _kmain                   /* Hand over to C++ */
_loop:                          /* Infinite loop in case we exit out of C++ */
  cli
  hlt
  jmp _loop

.section .bss
.space 2*1024*1024              /* Add 2MiB padding */
stack:                          /* Will point to some available space in RAM */
