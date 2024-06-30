#include "drivers/vga.hpp"

#include <kernel/arch/x86/interrupts.hpp>

namespace kernel
{

InterruptManager *InterruptManager::active_interrupt_manager = nullptr;

InterruptHandler::InterruptHandler(
    u8 interrupt_number,
    InterruptManager *interrupt_manager
)
    : m_interrupt_number(interrupt_number)
    , m_interrupt_manager(interrupt_manager)
{
    interrupt_manager->m_handlers[interrupt_number] = this;
}

InterruptHandler::~InterruptHandler()
{
    if (m_interrupt_manager->m_handlers[m_interrupt_number] == this)
    {
        m_interrupt_manager->m_handlers[m_interrupt_number] = nullptr;
    }
}

u32 InterruptHandler::handle_interrupt(u32 esp)
{
    return esp;
}

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
        m_handlers[i] = nullptr;
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

    // Each x86 system has two PICs, a "master" and "slave"
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

void InterruptManager::activate()
{
    if (active_interrupt_manager != nullptr)
    {
        active_interrupt_manager->deactivate();
    }
    active_interrupt_manager = this;
    asm volatile("sti");
}

void InterruptManager::deactivate()
{
    if (active_interrupt_manager == this)
    {
        active_interrupt_manager = nullptr;
        asm volatile("cli");
    }
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
    if (active_interrupt_manager != nullptr)
    {
        return active_interrupt_manager->do_handle_interrupt(
            interrupt_number, esp
        );
    }
    else
    {
        return esp;
    }
}

u32 InterruptManager::do_handle_interrupt(u8 interrupt_number, u32 esp)
{
    if (m_handlers[interrupt_number] != nullptr)
    {
        esp = m_handlers[interrupt_number]->handle_interrupt(esp);
    }
    // Print log if it's NOT a timer interrupt
    else if (interrupt_number != 0x20)
    {
        drivers::WRITER.new_line();
        drivers::WRITER.put_string("*** UNHANDLED INTERRUPT ***\n");
        drivers::WRITER.put_string("-> 0x");
        drivers::WRITER.put_integer_with_radix(
            drivers::VgaWriter::Unsigned, interrupt_number, 16
        );
        drivers::WRITER.put_string(" 0x");
        drivers::WRITER.put_integer_with_radix(
            drivers::VgaWriter::Unsigned, esp, 16
        );
        drivers::WRITER.new_line();
    }

    if (0x20 <= interrupt_number && interrupt_number <= 0x30)
    {
        pic_master_command.write(0x20);
        // Write to the slave PIC if the interrupt came from another slave PIC
        // (it has an interrupt number in the range 0x28..=0x30)
        if (0x28 <= interrupt_number)
        {
            pic_slave_command.write(0x20);
        }
    }

    return esp;
}

} // namespace kernel
