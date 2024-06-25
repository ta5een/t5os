#include <kernel/platform/x86/port.hpp>

// NOLINTBEGIN(hicpp-no-assembler)
namespace kernel
{

Port::Port(u16 port_number)
    : m_port_number(port_number)
{
}

Port8Bit::Port8Bit(u16 port_number)
    : Port(port_number)
{
}

void Port8Bit::write(u8 data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(m_port_number));
}

u8 Port8Bit::read()
{
    u8 result = 0;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(m_port_number));
    return result;
}

Port8BitSlow::Port8BitSlow(u16 port_number)
    : Port8Bit(port_number)
{
}

void Port8BitSlow::write(u8 data)
{
    asm volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                 :
                 : "a"(data), "Nd"(m_port_number));
}

Port16Bit::Port16Bit(u16 port_number)
    : Port(port_number)
{
}

void Port16Bit::write(u16 data)
{
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(m_port_number));
}

u16 Port16Bit::read()
{
    u16 result = 0;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(m_port_number));
    return result;
}

Port32Bit::Port32Bit(u16 port_number)
    : Port(port_number)
{
}

void Port32Bit::write(u32 data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(m_port_number));
}

u32 Port32Bit::read()
{
    u32 result = 0;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(m_port_number));
    return result;
}

} // namespace kernel

// NOLINTEND(hicpp-no-assembler)
