#pragma once

#include <kernel/arch/x86/gdt.h>
#include <stddef.h>
#include <stdint.h>

#define IDT_NUM_ENTRIES 0x100U

/**
 * An entry in the Interrupt Descriptor Table, representing an Interrupt Gate,
 * a Trap Gate or a Task Gate.
 *
 * NOTE: This is specific to IA-32.
 */
struct [[gnu::packed]] ia32_idt_entry
{
    uint16_t base_0_15;
    enum gdt_selector segment_selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_16_31;
};

// NOLINTNEXTLINE(readability-magic-numbers)
static_assert((bool)(sizeof(struct ia32_idt_entry) == 8U));

enum ia32_idt_flag : uint8_t
{
    IDT_FLAG_GATE_TASK = 0b0101U,
    IDT_FLAG_GATE_INT_16 = 0b0110U,
    IDT_FLAG_GATE_TRAP_16 = 0b0111U,
    IDT_FLAG_GATE_INT_32 = 0b1110U,
    IDT_FLAG_GATE_TRAP_32 = 0b1111U,
    IDT_FLAG_RING0 = (0U << 5U),
    IDT_FLAG_RING1 = (1U << 5U),
    IDT_FLAG_RING2 = (2U << 5U),
    IDT_FLAG_RING3 = (3U << 5U),
    IDT_FLAG_PRESENT = (1U << 7U),
};

// NOLINTNEXTLINE(readability-magic-numbers)
static_assert((bool)(sizeof(struct ia32_idt_entry) == 8U));

// FIXME: ISR handlers expect a pointer to a `struct interrupt_frame`.
typedef void(ia32_idt_handler_t)(void);

/*
 * Initializes the gates of the Interrupt Descriptor Table.
 */
void
ia32_idt_init();

/**
 * Modifies the contents of an IDT gate corresponding to the provided vector.
 */
void
ia32_idt_set_entry(
    size_t vector,
    ia32_idt_handler_t handler,
    enum gdt_selector segment_selector,
    enum ia32_idt_flag flags
);

// /**
//  * Starts listening to interrupts.
//  */
// void
// ia32_idt_activate();

// /**
//  * Stops listening to interrupts.
//  */
// void
// ia32_idt_deactivate();
