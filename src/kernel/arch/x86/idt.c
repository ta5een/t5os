#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>
#include <kernel/arch/x86/ports.h>
#include <stddef.h>
#include <stdint.h>

#define PORT_PIC_MASTER_COMMAND (0x20U)
#define PORT_PIC_MASTER_DATA    (0x21U)
#define PORT_PIC_SLAVE_COMMAND  (0xa0U)
#define PORT_PIC_SLAVE_DATA     (0xa1U)

static struct idt_entry s_idt[IDT_NUM_ENTRIES];

static struct descriptor_table_register s_idtr = {
    .limit = sizeof(s_idt) - 1,
    .base = (void *)s_idt,
};

void
idt_set_entry(
    size_t index,
    size_t segment_selector,
    void (*handler)(),
    enum idt_flag flags
)
{
    s_idt[index] = (struct idt_entry){
        .reserved = 0,
        .base_0_15 = ((uint32_t)handler) & 0xffffU,
        .base_16_31 = (((uint32_t)handler) >> 16U) & 0xffffU,
        .segment_selector = segment_selector,
        .flags = IDT_FLAG_PRESENT | flags,
    };
}

void
idt_init()
{
    for (size_t interrupt = 0; interrupt < IDT_NUM_ENTRIES; interrupt++)
    {
        idt_set_entry(
            interrupt,
            GDT_IDX_KCODE,
            &idt_ignore_interrupt_request,
            IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
        );
    }

    idt_set_entry(
        0x20,
        GDT_IDX_KCODE,
        &idt_handle_interrupt_request_0x00,
        IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    );

    idt_set_entry(
        0x21,
        GDT_IDX_KCODE,
        &idt_handle_interrupt_request_0x01,
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
idt_load()
{
    asm volatile("lidt %0" : : "m"(s_idtr) : "memory");
}

void
idt_activate()
{
    // TODO: Assumes there is one processor to listen to interrupts
    asm volatile("sti");
}

void
idt_deactivate()
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
