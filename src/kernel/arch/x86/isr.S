	/* Stub setup for Interrupt Services Routines (ISRs).
	 *
	 * ISRs follow a different protocol when they are invoked. Specifically, an
	 * ISR must end with the IRET opcode. This differs from regular C functions,
	 * which end with the RET or RETF opcode. Consequently, it is not possible
	 * to implement these handlers in C (at least without limited compiler-
	 * specific workarounds).
	 *
	 * To define an ISR for all exceptions and a select number of user-defined
	 * interrupts, this source file provides the following macros:
	 *
	 * - DEFINE_ISR_ERR, for interrupts that push an error code
	 * - DEFINE_ISR_NO_ERR, for interrupts that don't push an error code
	 *
	 * Hardware requests (IRQs) are not defined in this file. Please refer to
	 * "irq.S" instead.
	 *
	 * A majority of this code has been adapted from nanoybyte_os:
	 * - https://youtu.be/lWhyXLXflt4?si=Ixg_pBkZ2S1GqjBW
	 * - https://github.com/nanobyte-dev/nanobyte_os/blob/fe61d210896d05c98a731b2d77cded21c4d2c45f/src/kernel/arch/i686/isr_asm.asm
	 */

	.code32
	.section .text
	.extern i686_isr_handler

	/* Defines an ISR for interrupts that do not push an error code.
	 *
	 * In x86 protected mode, some exceptions and hardware interrupts do not
	 * push an error code onto the stack. This macro handles such interrupts by
	 * manually pushing a dummy error code onto the stack. This ensures that the
	 * stack layout remains consistent, with an error code always present,
	 * simplifying the interrupt handling process.
	 */
	.macro DEFINE_ISR_NO_ERR num
		.align 16
		.global i686_isr_\num
		i686_isr_\num:
		push $0					# Push dummy error code
		push $\num				# Push interrupt number
		jmp i686_isr_stub
	.endm

	/* Defines an ISR for interrupts that push an error code.
	 *
	 * In x86 protected mode, certain exceptions automatically push an error
	 * code onto the stack before transferring control to the interrupt handler.
	 * This macro is used for such interrupts.
	 */
	.macro DEFINE_ISR_ERR num
		.align 16
		.global i686_isr_\num
		i686_isr_\num:
								# CPU pushes error code here
		push $\num				# Push interrupt number
		jmp i686_isr_stub
	.endm

	DEFINE_ISR_NO_ERR	0x00
	DEFINE_ISR_NO_ERR	0x01
	DEFINE_ISR_NO_ERR	0x02
	DEFINE_ISR_NO_ERR	0x03
	DEFINE_ISR_NO_ERR	0x04
	DEFINE_ISR_NO_ERR	0x05
	DEFINE_ISR_NO_ERR	0x06
	DEFINE_ISR_NO_ERR	0x07
	DEFINE_ISR_ERR		0x08
	DEFINE_ISR_NO_ERR	0x09
	DEFINE_ISR_ERR		0x0a
	DEFINE_ISR_ERR		0x0b
	DEFINE_ISR_ERR		0x0c
	DEFINE_ISR_ERR		0x0d
	DEFINE_ISR_ERR		0x0e
	DEFINE_ISR_NO_ERR	0x0f
	DEFINE_ISR_NO_ERR	0x10
	DEFINE_ISR_ERR		0x11
	DEFINE_ISR_NO_ERR	0x12
	DEFINE_ISR_NO_ERR	0x13
	DEFINE_ISR_NO_ERR	0x14
	DEFINE_ISR_NO_ERR	0x15
	DEFINE_ISR_NO_ERR	0x16
	DEFINE_ISR_NO_ERR	0x17
	DEFINE_ISR_NO_ERR	0x18
	DEFINE_ISR_NO_ERR	0x19
	DEFINE_ISR_NO_ERR	0x1a
	DEFINE_ISR_NO_ERR	0x1b
	DEFINE_ISR_NO_ERR	0x1c
	DEFINE_ISR_NO_ERR	0x1d
	DEFINE_ISR_NO_ERR	0x1e	# Apparently this has an error code (OSDev Wiki)
	DEFINE_ISR_NO_ERR	0x1f
	# Start user-defined interrupts
	# DEFINE_ISR_NO_ERR	0x20
	# DEFINE_ISR_NO_ERR	0x80

i686_isr_stub:
	/* Store all general-purpose registers in the following order:
	 * EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
	 */
	pusha

	/* Save the current data segment.
	 *
	 * Since we don't use segmentation, all the extra segment registers (ES, FS,
	 * and GS), will have the same value as DS.
	 */
	xor %eax, %eax
	mov %ds, %ax
	push %eax

	/* Load the kernel data segment.
	 *
	 * `0x10` refers to segment selector of the third entry in the GDT:
	 *
	 *    0x00: Null Descriptor
	 *    0x08: Kernel Mode Code Segment
	 *    0x10: Kernel Mode Data Segment
	 */
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	/* Call the C handler.
	 *
	 * The stack at this point holds information we've collected so far. To
	 * pass this to the C function, we'll push a pointer to the stack before
	 * calling the function.
	 *
	 * Once we return from the C function, we'll make sure to remove the
	 * argument from the stack.
	 */
	push %esp
	call i686_isr_handler
	add $4, %esp

	/* Restore the original data segment. */
	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	/* Restore the general-purpose registers prior to loading the kernel data
	 * segment.
	 */
	popa

	/* Remove the error code and interrupt number from the stack before finally
	 * returning from the interrupt with IRET.
	 */
	add $8, %esp
	iret

	.global i686_isr_unhandled_stub
i686_isr_unhandled_stub:
	/* Halt the OS for now */
	cli
	hlt
