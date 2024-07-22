    /* Subroutines for the Global Descriptor Table (GDT).
     *
     * A majority of this code has been adapted from nanobyte_os:
     * - https://youtu.be/5LbXClJhxcs?si=gyTvJUt7HVz9NwU6
     * - https://github.com/nanobyte-dev/nanobyte_os/blob/fe61d210896d05c98a731b2d77cded21c4d2c45f/src/kernel/arch/i686/gdt_asm.asm
     */

    .code32
    .section .text

    /* Loads the GDT and sets the segment registers to start using it.
     *
     * @param const struct x86_descriptor_table_register *gdtr
     * @param uint32_t code_segment_selector
     * @param uint32_t data_segment_selector
     * @returns void
     */
    .global x86_32_gdt_load
    .type x86_32_gdt_load, @function
x86_32_gdt_load:
    /* Prologue: make a new call frame for this subroutine. */
    push %ebp                       # Save the caller's base pointer value
    mov %esp, %ebp                  # Set the new base pointer value

    /* Load the GDT with address of the descriptor table register (gdtr). */
    mov 8(%ebp), %eax               # Push 1st arg (gdtr)
    lgdt (%eax)                     # Load GDT with the address of eax (1st arg)

    /* Reload the code segment.
     *
     * Due to x86 segmentation rules, the CPU requires a far return (RETF) to
     * switch code segments. The RETF instruction expects a code segment
     * selector and a return instruction pointer at the top of the stack. Once
     * the RETF instruction is executed, the CPU will have switched to the new
     * code segment and will start executing instructions at the new instruction
     * pointer within this new context.
     *
     * After `x86_32_gdt_load` returns with `ret`, execution continues at the
     * instruction following the `x86_32_gdt_load` call, using the new code
     * segment specified by `code_segment_selector`.
     */
    mov 12(%ebp), %eax              # Push 2nd arg (code_segment_selector)
    push %eax
    push $reload_cs                 # Push address to label reload_cs
    retf
reload_cs:

    /* Reload the data segment. */
    mov 16(%ebp), %ax               # Push 3rd arg (data_segment_selector)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    /* Epilogue: restore the caller's call frame */
    mov %ebp, %esp                  # Deallocate the local variable
    pop %ebp                        # Restore the caller's base pointer value
    ret
