#include <drivers/keyboard.hpp>
#include <drivers/vga.hpp>
#include <kernel/arch/x86/interrupts.hpp>

namespace drivers
{

KeyboardDriver::KeyboardDriver(kernel::InterruptManager *manager)
    : kernel::InterruptHandler(0x21, manager)
    , m_data_port(0x60)
    , m_command_port(0x64)
{
    // Flush all previous key interrupts
    while (m_command_port.read() & 0x1)
    {
        m_data_port.read();
    }

    m_command_port.write(0xae); // Activate keyboard interrupts
    m_command_port.write(0x20); // Get current state

    u8 status = (m_data_port.read() | 1U) & ~0x10U;
    m_command_port.write(0x60); // Set state
    m_data_port.write(status);

    m_data_port.write(0xf4); // Activate the keyboard
}

KeyboardDriver::~KeyboardDriver() {}

u32 KeyboardDriver::handle_interrupt(u32 esp)
{
    const auto &vga = VgaWriter::instance();
    u8 keycode = m_data_port.read();

    // Only care about key-down, not key-up (set as the 5th bit?)
    if (keycode < 0x80)
    {
        switch (keycode)
        {
        // Command acknowledged (ACK)
        case 0xfa:
            break;
        // case 0x1e:
        //     WRITER.put_string("a");
        //     break;
        default:
            vga.new_line();
            vga.put_string("*** KEYBOARD INTERRUPT ***\n");
            vga.put_string("-> 0x");
            vga.put_integer_with_radix(VgaWriter::Unsigned, keycode, 16);
            vga.new_line();
            break;
        }
    }

    return esp;
}

} // namespace drivers
