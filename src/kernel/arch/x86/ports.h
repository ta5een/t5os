#pragma once

#include <stdint.h>

uint8_t
port_read_8(uint16_t port_number);

void
port_write_8(uint16_t port_number, uint8_t data);

uint16_t
port_read_16(uint16_t port_number);

void
port_write_16(uint16_t port_number, uint16_t data);

uint32_t
port_read_32(uint16_t port_number);

void
port_write_32(uint16_t port_number, uint32_t data);
