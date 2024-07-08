#include <kernel/arch/x86/devices/serial.h>
#include <kernel/arch/x86/ports.h>

#define SERIAL_DATA_PORT(base)          ((uint16_t)base)
#define SERIAL_FIFO_COMMAND_PORT(base)  ((uint16_t)base + 2U)
#define SERIAL_LINE_COMMAND_PORT(base)  ((uint16_t)base + 3U)
#define SERIAL_MODEM_COMMAND_PORT(base) ((uint16_t)base + 4U)
#define SERIAL_LINE_STATUS_PORT(base)   ((uint16_t)base + 5U)

/**
 * Enable the Divsor Latch Access Bit (DLAB).
 *
 * Tells the serial port to first expect the highest 8 bits on the data port,
 * then the lowest 8 bits will follow.
 */
#define SERIAL_LINE_ENABLE_DLAB ((uint8_t)0x80U)

/**
 * Sets the speed of the data being sent.
 *
 * The default speed of a serial port is 115'200 bits/sec. The argument is a
 * divisor of that number, hence the resulting speed becomes (115'200 /
 * divisor) bits/sec.
 */
static void serial_configure_baud_rate(uint16_t com_port, uint16_t divisor)
{
    port_write_8(SERIAL_LINE_COMMAND_PORT(com_port), SERIAL_LINE_ENABLE_DLAB);
    port_write_8(SERIAL_DATA_PORT(com_port), (divisor >> 8U) & 0x00ffU);
    port_write_8(SERIAL_DATA_PORT(com_port), divisor & 0x00ffU);
}

/**
 * Configures the line of the given serial port.
 *
 * The port is set to have a data length of 8 bits, no parity bits, one stop
 * bit, and break control disabled.
 */
static void serial_configure_line(uint16_t com_port)
{
    // The following table and accompanying legend describes the structure of
    // the bits to configure the serial port: [1][2]
    //
    // Table:
    //   .__________.___.___._______.___._____.
    //   | Bit      | 7 | 6 | 5 4 3 | 2 | 1 0 |
    //   | Content  | d | b | prty  | s | dl  |
    //   | Value    | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    //   '----------'---'---'-------'---'-----'
    //
    // Legend:
    //   - d: Enables (1) or disables (0) DLAB
    //   - b: Enables (1) or disables (0) break control
    //   - prty: The number of parity bits to use
    //   - s: The number of stop bits to use (0 means 1 bit, 1 means 1.5 or 2)
    //   - dl: Describes the length of the data (0 means 5 bits, 3 means 8 bits)
    //
    // [1]: https://littleosbook.github.io/#configuring-the-line
    // [2]: https://wiki.osdev.org/Serial_Ports
    port_write_8(SERIAL_LINE_COMMAND_PORT(com_port), 0x03U);
}

/**
 * Is the transmit FIFO queue is empty for the given serial port?
 */
static bool serial_is_transmit_fifo_empty(uint16_t com_port)
{
    uint8_t value = port_read_8(com_port + 5) & 0x20U;
    return (bool)(value != 0U);
}

enum serial_init_response serial_init(uint16_t com_port)
{
    // Disable all interrupts
    // NOTE: OSDev Wiki writes to SERIAL_DATA_PORT(com_port) + 1
    port_write_8(SERIAL_DATA_PORT(com_port), 0x00);

    // Set baud rate to 38'400
    serial_configure_baud_rate(com_port, 0x03);

    // 8 bits, no parity, one stop bit
    serial_configure_line(com_port);

    // Enable FIFO, clear them, with 14-byte threshold
    port_write_8(SERIAL_FIFO_COMMAND_PORT(com_port), 0xc7);

    // IRQs enabled, RTS/DSR set
    port_write_8(SERIAL_MODEM_COMMAND_PORT(com_port), 0x0b);

    // Set in loopback mode, test the serial chip
    port_write_8(SERIAL_MODEM_COMMAND_PORT(com_port), 0x1e);

    // Test serial chip (send byte 0xae and check if serial returns same byte)
    port_write_8(SERIAL_DATA_PORT(com_port), 0xae);

    // Check if serial is faulty (i.e. not the same byte as sent)
    if (port_read_8(SERIAL_DATA_PORT(com_port)) != 0xae)
    {
        return SERIAL_INIT_FAILURE;
    }

    // If serial is not faulty, set it in normal operation mode
    port_write_8(SERIAL_MODEM_COMMAND_PORT(com_port), 0x0f);

    return SERIAL_INIT_SUCCESS;
}

void serial_write(uint16_t com_port, string_view_t string)
{
    // WARN: Assumes the provided string slice is encoded in UTF-8
    for (size_t i = 0; i < string.length; i++)
    {
        // Wait for queue to be free
        while (!serial_is_transmit_fifo_empty(com_port))
        {
            asm volatile("pause");
        }
        port_write_8(SERIAL_DATA_PORT(com_port), string.data[i]);
    }
}
