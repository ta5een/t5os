#include "drivers/vga.hpp"

#include <kernel/platform/x86/interrupts.hpp>

namespace kernel
{

InterruptManager::GateDescriptor
    InterruptManager::interrupt_descriptor_table[256];

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt)
    : pic_master_command(0x20)
    , pic_master_data(0x21)
    , pic_slave_command(0xa0)
    , pic_slave_data(0xa1)
{
    u16 code_segment = gdt->code_segment_selector();
    const u8 IDT_INTERRUPT_GATE = 0xe;

    for (u16 i = 0; i < 256; i++)
    {
        set_interrupt_descriptor_table_entry(
            i, code_segment, &ignore_interrupt_request, 0, IDT_INTERRUPT_GATE
        );
    }

    set_interrupt_descriptor_table_entry(
        0x20, code_segment, &handle_interrupt_request0x00, 0, IDT_INTERRUPT_GATE
    );

    set_interrupt_descriptor_table_entry(
        0x21, code_segment, &handle_interrupt_request0x01, 0, IDT_INTERRUPT_GATE
    );

    pic_master_command.write(0x11);
    pic_slave_command.write(0x11);

    pic_master_data.write(0x20);
    pic_slave_data.write(0x28);

    pic_master_data.write(0x04);
    pic_slave_data.write(0x02);

    pic_master_data.write(0x01);
    pic_slave_data.write(0x01);

    pic_master_data.write(0x00);
    pic_slave_data.write(0x00);

    InterruptDescriptorTablePointer idt{};
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (u32)interrupt_descriptor_table;
    asm volatile("lidt %0" ::"m"(idt));
}

InterruptManager::~InterruptManager() = default;

void InterruptManager::activate()
{
    asm volatile("sti");
}

void InterruptManager::set_interrupt_descriptor_table_entry(
    u8 interrupt_number,
    u16 code_segment_selector_offset,
    void (*handler)(),
    u8 descriptor_privilege_level,
    u8 descriptor_type
)
{
    const u8 IDT_DESC_PRESENT = 0x80;

    interrupt_descriptor_table[interrupt_number].handler_address_lo =
        ((u32)handler) & 0xffff;
    interrupt_descriptor_table[interrupt_number].handler_address_hi =
        (((u32)handler) >> 16) & 0xffff;
    interrupt_descriptor_table[interrupt_number].gdt_code_segment_selector =
        code_segment_selector_offset;
    interrupt_descriptor_table[interrupt_number].access =
        IDT_DESC_PRESENT | descriptor_type |
        ((descriptor_privilege_level & 3) << 5);
    interrupt_descriptor_table[interrupt_number].reserved = 0;
}

u32 InterruptManager::handle_interrupt(u8 interrupt_number, u32 esp)
{
    drivers::WRITER.put_string("*** INTERRUPT ***\n");
    drivers::WRITER.put_string("-> ");
    drivers::WRITER.put_integer_with_radix(
        drivers::VgaWriter::Unsigned, interrupt_number, 16
    );
    drivers::WRITER.put_string(" 0x");
    drivers::WRITER.put_integer_with_radix(
        drivers::VgaWriter::Unsigned, esp, 16
    );
    drivers::WRITER.new_line();

    return esp;
}

} // namespace kernel
