#pragma once

#include <stddef.h>
#include <stdint.h>

#define IDT_NUM_ENTRIES 256

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
    uint8_t attributes;
    uint16_t base_16_31;
};

// NOLINTNEXTLINE(readability-*)
static_assert(sizeof(struct idt_entry) == 8U);

struct idt
{
    struct idt_entry entries[IDT_NUM_ENTRIES];
};

// NOLINTNEXTLINE(readability-*)
static_assert(sizeof(struct idt_entry) == 8U);

/*
 * Initialize the gates of the Interrupt Descriptor Table.
 */
void idt_init(struct idt *idt, size_t gdt_kernel_cs_selector);

/**
 * Load the Interrupt Descriptor Table with the LIDT instruction.
 */
void idt_load(const struct idt idt[static 1]);

/**
 * Start listening to interrupts.
 */
void idt_activate(const struct idt idt[static 1]);

/**
 * Stop listening to interrupts/
 */
void idt_deactivate(const struct idt idt[static 1]);

size_t idt_handle_interrupt(size_t interrupt_number, size_t esp);

extern void idt_ignore_interrupt_request();
void idt_handle_interrupt_request_0x00();
void idt_handle_interrupt_request_0x01();
