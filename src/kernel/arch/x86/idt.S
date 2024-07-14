	/* Stub setup for Interrupt Services Routines (ISRs).
	 *
	 * ISRs follow a different protocol when they are invoked. Specifically, an
	 * ISR must end with the `iret` opcode. This differs from regular C
	 * functions, which end with the `ret` or `retf` opcode. Consequently, it is
	 * not possible to implement these handlers in C (at least without limited
	 * compiler-specific workarounds).
	 *
	 * To define an ISR for each interrupt, this source file provides a macro
	 * for interrupts that don't push an error code (DEFINE_ISR_NO_ERR), a macro
	 * for interrupts that do push an error code (DEFINE_ISR_ERR), and a macro
	 * for interrupts triggered by the PIC (DEFINE_IRQ), which needs to be
	 * handled slightly differently.
	 */

	.code32
	.section .text
	.extern isr_handler
	.extern irq_handler

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
	.global isr_\num
	isr_\num:
		push $0					# Push dummy error code
		push $\num				# Push interrupt number
		jmp isr_stub
	.endm

	/* Defines an ISR for interrupts that push an error code.
	 *
	 * In x86 protected mode, certain exceptions automatically push an error
	 * code onto the stack before transferring control to the interrupt handler.
	 * This macro is used for such interrupts.
	 */
	.macro DEFINE_ISR_ERR num
	.align 16
	.global isr_\num
	isr_\num:
								# CPU pushes error code here
		push $\num				# Push interrupt number
		jmp isr_stub
	.endm

	# .macro DEFINE_IRQ num
	# .align 16
	# .global irq_\num
	# irq_\num:
	# 	push $\num				# Push interrupt number
	# 	jmp irq_stub
	# .endm

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
DEFINE_ISR_ERR		0x1e		# Apparently has en error code (OSDev Wiki)
DEFINE_ISR_NO_ERR	0x1f
# # Start user-defined interrupts
# DEFINE_ISR_NO_ERR	0x20
# DEFINE_ISR_NO_ERR	0x80

isr_stub:
	/* Store all general-purpose registers in the following order:
	 * eax, ecx, edx, ebx, esp, ebp, esi, edi
	 */
	pusha

	/* Save current data segment */
	xor %eax, %eax
	mov %ds, %ax
	push %eax

	/* Load the kernel data segment */
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	/* Push the address of the frame structure */
	push %esp
	/* Call the C handler */
	call isr_handler
	/* Pop the address of the frame structure */
	add $4, %esp

	/* Restore the original data segment */
	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	/* Restore general-purpose registers */
	popa
	/* Remove the error code and interrupt number from the stack */
	add $8, %esp

	/* Return from the interrupt */
	iret

	.global isr_unhandled_stub
isr_unhandled_stub:
	# iret
	cli
	hlt
