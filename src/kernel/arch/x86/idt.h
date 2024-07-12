#pragma once

#include <stddef.h>
#include <stdint.h>

#define IDT_ENTRY_ALIGN   0x10U
#define IDT_NUM_ENTRIES   0x100U
#define IDT_HANDLER_ALIGN 16U

/**
 * An entry in the Interrupt Descriptor Table, representing an Interrupt Gate,
 * a Trap Gate or a Task Gate.
 *
 * NOTE: This is specific to IA-32.
 */
struct [[gnu::packed]] idt_entry
{
    uint16_t base_0_15;
    uint16_t segment_selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_16_31;
};

// NOLINTNEXTLINE(readability-magic-numbers)
static_assert((bool)(sizeof(struct idt_entry) == 8U));

enum idt_flag : uint8_t
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
static_assert((bool)(sizeof(struct idt_entry) == 8U));

/*
 * Initializes the gates of the Interrupt Descriptor Table.
 */
void
idt_init();

/**
 * Loads the Interrupt Descriptor Table with the LIDT instruction.
 */
void
idt_load();

// /**
//  * Starts listening to interrupts.
//  */
// void
// idt_activate();

// /**
//  * Stops listening to interrupts.
//  */
// void
// idt_deactivate();
