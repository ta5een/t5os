#include <kernel/arch/x86/ports.h>
#include <stdint.h>

uint8_t inb(uint16_t port_number)
{
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}

void outb(uint16_t port_number, uint8_t data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port_number));
}

uint16_t inw(uint16_t port_number)
{
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}

void outw(uint16_t port_number, uint16_t data)
{
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(port_number));
}

uint32_t inl(uint16_t port_number)
{
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port_number));
    return result;
}

void outl(uint16_t port_number, uint32_t data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(port_number));
}
