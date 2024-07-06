#pragma once

#include <stdint.h>

uint8_t inb(uint16_t port_number);
void outb(uint16_t port_number, uint8_t data);

uint16_t inw(uint16_t port_number);
void outw(uint16_t port_number, uint16_t data);

uint32_t inl(uint16_t port_number);
void outl(uint16_t port_number, uint32_t data);
