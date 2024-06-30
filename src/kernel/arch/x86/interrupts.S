	# Setting up stubs for handling Interrupt Services Routines (ISRs).
	#
	# ISRs follow a different protocol when they are invoked. Specifically, an
	# ISR must end with the `iret` opcode. This differs from regular C/C++
	# functions, which end with the `ret` or `retf` opcode. Consequently, it is
	# not possible to implement these handlers in C/C++ (at least without
	# compiler-specific workarounds).
	#
	# This source file defines all the interrupts to be handled using macros.
	# Each handler will have instructions that properly adhere to the protocol,
	# and sandwiched in between, a call to a C++ function is inserted. This C++
	# function is declared as an external variable with a mangled name, though
	# in the future this will be replaced with its non-mangled name.

	.set IRQ_BASE, 0x20

	.section .text
	.extern _ZN6kernel16InterruptManager16handle_interruptEhj
	.global _ZN6kernel16InterruptManager24ignore_interrupt_requestEv

	.macro handle_exception num
	.global _ZN6kernel16InterruptManager16handle_exception\num\()Ev
	_ZN6kernel16InterruptManager16handle_exception\num\()Ev:
	movb  $\num, (interrupt_number)
	jmp   interrupt_request_handler_stub
	.endm

	.macro handle_interrupt_request num
	.global _ZN6kernel16InterruptManager28handle_interrupt_request\num\()Ev
	_ZN6kernel16InterruptManager28handle_interrupt_request\num\()Ev:
	movb  $\num + IRQ_BASE, (interrupt_number)
	jmp   interrupt_request_handler_stub
	.endm

	handle_interrupt_request 0x00
	handle_interrupt_request 0x01

interrupt_request_handler_stub:
	# Store current register values of this stack
	pusha
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs

	# Push interrupt number and stack pointer to this stack and call handler
	push  %esp
	push  (interrupt_number)
	call  _ZN6kernel16InterruptManager16handle_interruptEhj
	movl  %eax, %esp

	# Restore the previous register values back to this stack (in reverse order)
	popl  %gs
	popl  %fs
	popl  %es
	popl  %ds
	popa

_ZN6kernel16InterruptManager24ignore_interrupt_requestEv:
	# Tell the CPU we are finished handling the interrupt
	iret

	.data
interrupt_number: .byte 0
