#pragma once

#include <libcore/string_view.h>
#include <stdint.h>

#define SERIAL_COM1_BASE 0x3f8 // COM1 base port

enum serial_init_response
{
    SERIAL_INIT_SUCCESS = 0,
    SERIAL_INIT_FAILURE = 1,
};

/**
 * Initializes the serial port with the provided COM base port.
 *
 * Returns the following based on the intialization outcome:
 *
 * - `SERIAL_INIT_SUCCESS`: The port is successfully initialized.
 * - `SERIAL_INIT_FAILURE`: The port failed the post-initialization test.
 *
 * TODO: Consider creating instances of serial ports to wrap in a mutex.
 */
[[nodiscard]]
enum serial_init_response
serial_init(uint16_t com_port);

/**
 * Writes a string to the given serial port.
 *
 * TODO: Write a macro to call this function with a default COM port and
 * formattable arguments.
 *
 * TODO: Consider wrapping this operation in a no-interrupt scope.
 */
void
serial_write(uint16_t com_port, string_view_t string);
