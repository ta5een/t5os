#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/devices/vga.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>
#include <kernel/arch/x86/ports.h>
#include <stddef.h>
#include <stdint.h>

#define PORT_PIC_MASTER_COMMAND (0x20U)
#define PORT_PIC_MASTER_DATA    (0x21U)
#define PORT_PIC_SLAVE_COMMAND  (0xa0U)
#define PORT_PIC_SLAVE_DATA     (0xa1U)

[[gnu::aligned(IDT_ENTRY_ALIGN)]]
static struct idt_entry s_idt[IDT_NUM_ENTRIES];

static struct descriptor_table_register s_idtr = {
    .limit = sizeof(s_idt) - 1,
    .base = (void *)s_idt,
};

extern uint8_t isr_0x00[];

void
idt_set_entry(
    size_t index,
    size_t segment_index,
    uint32_t handler,
    enum idt_flag flags
)
{
    s_idt[index] = (struct idt_entry){
        .base_0_15 = handler & 0xffffU,
        .base_16_31 = (handler >> 16U) & 0xffffU,
        .segment_selector = segment_index * sizeof(struct gdt_entry),
        .flags = IDT_FLAG_PRESENT | flags,
        .reserved = 0,
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
            (uint32_t)isr_0x00 + (interrupt * 16U),
            IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
        );
    }

    // idt_set_entry(
    //     0x20,
    //     GDT_IDX_KCODE,
    //     &idt_handle_interrupt_request_0x00,
    //     IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    // );
    //
    // idt_set_entry(
    //     0x21,
    //     GDT_IDX_KCODE,
    //     &idt_handle_interrupt_request_0x01,
    //     IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    // );

    // port_write_8(PORT_PIC_MASTER_COMMAND, 0x11U);
    // port_write_8(PORT_PIC_SLAVE_COMMAND, 0x11U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x20U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x28U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x04U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x02U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x01U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x01U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x00U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x00U);
}

void
idt_load()
{
    asm volatile("lidt %0" : : "m"(s_idtr) : "memory");
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

struct [[gnu::packed]] registers
{
    // Data segment, manually pushed by us
    size_t ds;
    // Pushed with PUSHA, listed here in reverse order
    size_t edi, esi, ebp, kernel_esp, ebx, edx, ecx, eax;
    // Manually pushed by us
    size_t interrupt_number;
    // Manually pushed by us
    size_t error_code;
    // Pushed automatically by the CPU, listed here in reverse order
    size_t eip, cs, eflags, esp, ss;
};

[[gnu::cdecl]]
void
isr_handler(struct registers *registers)
{
    struct vga *vga = vga_get();
    switch (registers->interrupt_number)
    {
    case 0x02:
        vga_println(vga, "INT 0x02");
        break;
    case 0x03:
        vga_println(vga, "INT 0x03");
        break;
    case 0x04:
        vga_println(vga, "INT 0x04");
        break;
    default:
        vga_println(vga, "INT ????");
        break;
    }
}
