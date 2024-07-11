#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/idt.h>
#include <kernel/arch/x86/ports.h>
#include <stdint.h>

#define PORT_PIC_MASTER_COMMAND (0x20U)
#define PORT_PIC_MASTER_DATA    (0x21U)
#define PORT_PIC_SLAVE_COMMAND  (0xa0U)
#define PORT_PIC_SLAVE_DATA     (0xa1U)

void
idt_set_entry(
    struct idt_entry target[static 1],
    size_t segment_selector,
    uint32_t handler,
    enum idt_flag flags
)
{
    target->reserved = 0;
    target->base_0_15 = handler & 0xffffU;
    target->base_16_31 = (handler >> 16U) & 0xffffU;
    target->segment_selector = segment_selector;
    target->flags = IDT_FLAG_PRESENT | flags;
}

void
idt_init(struct idt *idt, size_t segment_selector)
{
    for (size_t interrupt = 0; interrupt < IDT_NUM_ENTRIES; interrupt++)
    {
        idt_set_entry(
            &idt->entries[interrupt],
            segment_selector,
            (uint32_t)&idt_ignore_interrupt_request,
            IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
        );
    }

    idt_set_entry(
        &idt->entries[0x20],
        segment_selector,
        (uint32_t)&idt_handle_interrupt_request_0x00,
        IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    );

    idt_set_entry(
        &idt->entries[0x21],
        segment_selector,
        (uint32_t)&idt_handle_interrupt_request_0x01,
        IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    );

    port_write_8(PORT_PIC_MASTER_COMMAND, 0x11U);
    port_write_8(PORT_PIC_SLAVE_COMMAND, 0x11U);

    port_write_8(PORT_PIC_MASTER_DATA, 0x20U);
    port_write_8(PORT_PIC_SLAVE_DATA, 0x28U);

    port_write_8(PORT_PIC_MASTER_DATA, 0x04U);
    port_write_8(PORT_PIC_SLAVE_DATA, 0x02U);

    port_write_8(PORT_PIC_MASTER_DATA, 0x01U);
    port_write_8(PORT_PIC_SLAVE_DATA, 0x01U);

    port_write_8(PORT_PIC_MASTER_DATA, 0x00U);
    port_write_8(PORT_PIC_SLAVE_DATA, 0x00U);
}

void
idt_load(const struct idt idt[static 1])
{
    struct descriptor_table_register idtr;
    idtr.limit = sizeof(struct idt) - 1;
    idtr.base = (void *)idt;
    asm volatile("lidt %0" : : "m"(idtr) : "memory");
}

void
idt_activate(const struct idt idt[static 1])
{
    // TODO: Assumes there is one processor to listen to interrupts
    asm volatile("sti");
}

void
idt_deactivate(const struct idt idt[static 1])
{
    // TODO: Assumes there is one processor to stop listening to interrupts
    asm volatile("cli");
}

size_t
idt_handle_interrupt(uint8_t interrupt_number, size_t esp)
{
    if (0x20 <= interrupt_number && interrupt_number <= 0x30)
    {
        port_write_8(PORT_PIC_MASTER_COMMAND, 0x20);
        if (0x28 <= interrupt_number)
        {
            port_write_8(PORT_PIC_SLAVE_COMMAND, 0x20);
        }
    }

    return esp;
}
