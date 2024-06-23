/* Comments retrieved from:
   https://gitlab.com/sortie/meaty-skeleton/-/blob/084d1624bedaa9f9e395f055c6bd99299bd97f58/kernel/arch/i386/linker.ld */
ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

SECTIONS
{
  /* Begin putting sections at 1MiB, a conventional place for kernels to be
     loaded at by the bootloader. */
  . = 1M;

  /* First, put the multiboot header, as it is required to be put very early in
     the image. Otherwise, the bootloader won't recognize the file format.
     Next, put the .text section, which provides the C++ code for _kmain and
     call_ctors. */
  .text :
  {
    *(.multiboot)
    *(.text*)
  }

  /* Read-only data. */
  .rodata :
  {
    *(.rodata)
  }

  /* Read-write data (initialized). */
  .data :
  {
    start_ctors = .;
    KEEP(*( .init_array ))
    KEEP(*( SORT_BY_INIT_PRIORITY( .init_array.* ) ))
    end_ctors = .;

    *(.data)
  }

  /* Read-write data (uninitialized) and stack. */
  .bss :
  {
    *(.bss)
  }

  /DISCARD/ :
  {
    *(.fini_array*)
    *(.comment)
  }
}
