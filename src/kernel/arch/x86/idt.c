#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/idt.h>
#include <kernel/arch/x86/ports.h>
#include <stdint.h>

#define PORT_PIC_MASTER_COMMAND 0x20U
#define PORT_PIC_MASTER_DATA    0x21U
#define PORT_PIC_SLAVE_COMMAND  0xa0U
#define PORT_PIC_SLAVE_DATA     0xa1U

void
idt_set_entry(
    struct idt_entry target[static 1],
    size_t gdt_kernel_cs_selector,
    uint32_t handler,
    uint8_t dpl,
    uint8_t type
)
{
    target->reserved = 0;
    target->base_0_15 = handler & 0xffffU;
    target->base_16_31 = (handler >> 16) & 0xffffU;
    target->segment_selector = gdt_kernel_cs_selector;
    target->attributes = (1U << 7U) | type | ((dpl & 3U) << 5U);
}

void
idt_init(struct idt *idt, size_t gdt_kernel_cs_selector)
{
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

    for (size_t interrupt = 0; interrupt < IDT_NUM_ENTRIES; interrupt++)
    {
        idt_set_entry(
            &idt->entries[interrupt],
            gdt_kernel_cs_selector,
            (uint32_t)&idt_ignore_interrupt_request,
            0,
            0xe
        );
    }

    idt_set_entry(
        &idt->entries[0x20],
        gdt_kernel_cs_selector,
        (uint32_t)&idt_handle_interrupt_request_0x00,
        0,
        0xe
    );

    idt_set_entry(
        &idt->entries[0x21],
        gdt_kernel_cs_selector,
        (uint32_t)&idt_handle_interrupt_request_0x01,
        0,
        0xe
    );
}

void
idt_load(const struct idt idt[static 1])
{
    struct descriptor_table_register idtr;
    idtr.limit = (sizeof(struct idt_entry) * IDT_NUM_ENTRIES) - 1;
    idtr.base = (void *)idt->entries;
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
idt_handle_interrupt(size_t interrupt_number, size_t esp)
{
    return esp;
}

void
idt_handle_interrupt_request_0x00() {};

void
idt_handle_interrupt_request_0x01() {};
