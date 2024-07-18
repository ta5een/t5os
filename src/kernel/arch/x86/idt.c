#include "idt.h"

#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/devices/vga.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/ports.h>
#include <stddef.h>
#include <stdint.h>

#define PORT_PIC_MASTER_COMMAND (0x20U)
#define PORT_PIC_MASTER_DATA    (0x21U)
#define PORT_PIC_SLAVE_COMMAND  (0xa0U)
#define PORT_PIC_SLAVE_DATA     (0xa1U)

static struct i686_idt_entry s_idt[IDT_NUM_ENTRIES];

static const struct descriptor_table_register s_idtr = {
    .limit = sizeof(s_idt) - 1,
    .base = (void *)s_idt,
};

/**
 * Loads the Interrupt Descriptor Table with the LIDT instruction.
 */
void
i686_idt_load()
{
    asm volatile("lidt %0" : : "m"(s_idtr) : "memory");
}

void
i686_idt_init()
{
    i686_idt_load();
}

void
i686_idt_set_entry(
    size_t vector,
    i686_idt_handler_t handler,
    enum gdt_selector segment_selector,
    enum idt_flag flags
)
{
    s_idt[vector] = (struct i686_idt_entry){
        .base_0_15 = ((uint32_t)handler) & 0xffffU,
        .base_16_31 = ((uint32_t)handler >> 16U) & 0xffffU,
        .segment_selector = segment_selector,
        .flags = IDT_FLAG_PRESENT | flags,
        .reserved = 0,
    };
}

// void
// idt_activate()
// {
//     // TODO: Assumes there is one processor to listen to interrupts
//     asm volatile("sti");
// }

// void
// idt_deactivate()
// {
//     // TODO: Assumes there is one processor to stop listening to interrupts
//     asm volatile("cli");
// }
