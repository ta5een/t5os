#include "kernel/arch/port.h"
#include <stdint.h>

uint8_t
x86_port_read_8(uint16_t port_number)
{
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}

void
x86_port_write_8(uint16_t port_number, uint8_t data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port_number));
}

uint16_t
x86_port_read_16(uint16_t port_number)
{
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}

void
x86_port_write_16(uint16_t port_number, uint16_t data)
{
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(port_number));
}

uint32_t
x86_port_read_32(uint16_t port_number)
{
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}

void
x86_port_write_32(uint16_t port_number, uint32_t data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(port_number));
}
