/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(loader)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
  /* Begin putting sections at 2MiB.

     It used to be universally recommended to use 1MiB as a start offset, as it
     was guaranteed to be available under BIOS systems. However, UEFI has made
     things more complicated, and experimental data strongly suggests that 2M
     is a safer place to load. [citation needed] */
  . = 2M;

  /* Executable code.

     First, put the multiboot header, as it is required to be put very early in
     the image or else the bootloader won't recognize the file format.

     Next, put the .text section, which provides the C++ code for _kmain and
     call_ctors. */
  .text BLOCK(4K) : ALIGN(4K)
  {
    *(.multiboot)
    *(.text*)
  }

  /* Read-only data. */
  .rodata BLOCK(4K) : ALIGN(4K)
  {
    *(.rodata)
  }

  /* Read-write data (initialized). */
  .data BLOCK(4K) : ALIGN(4K)
  {
    start_ctors = .;
    KEEP(*( .init_array ))
    KEEP(*( SORT_BY_INIT_PRIORITY( .init_array.* ) ))
    end_ctors = .;

    *(.data)
  }

  /* Read-write data (uninitialized) and stack. */
  .bss BLOCK(4K) : ALIGN(4K)
  {
    *(COMMON)
    *(.bss)
  }

  /DISCARD/ :
  {
    *(.fini_array*)
    *(.comment)
  }
}
