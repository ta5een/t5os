#pragma once

#include <stdint.h>

uint8_t
x86_port_read_8(uint16_t port_number);

void
x86_port_write_8(uint16_t port_number, uint8_t data);

uint16_t
x86_port_read_16(uint16_t port_number);

void
x86_port_write_16(uint16_t port_number, uint16_t data);

uint32_t
x86_port_read_32(uint16_t port_number);

void
x86_port_write_32(uint16_t port_number, uint32_t data);
