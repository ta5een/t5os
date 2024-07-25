#include <kernel/arch/x86/devices/serial.h>
#include <kernel/arch/x86/port.h>
#include <stdint.h>

#define SERIAL_DATA_REG(base)          ((uint16_t)base)
#define SERIAL_FIFO_CONTROL_REG(base)  ((uint16_t)base + 2U)
#define SERIAL_LINE_CONTROL_REG(base)  ((uint16_t)base + 3U)
#define SERIAL_MODEM_CONTROL_REG(base) ((uint16_t)base + 4U)
#define SERIAL_LINE_STATUS_REG(base)   ((uint16_t)base + 5U)

/**
 * Enable the Divisor Latch Access Bit (DLAB).
 *
 * Tells the serial port to expect the most significant byte first before
 * the least significant byte.
 *
 * TODO: Does this apply to both sending and receiving?
 */
#define SERIAL_LINE_ENABLE_DLAB ((uint8_t)0x80U)

/**
 * Arbitrary byte used to test loopback response of a serial port.
 */
#define SERIAL_TEST_BYTE ((uint8_t)0xaeU)

/**
 * Sets the speed of the data being sent.
 *
 * The default speed of a serial port is 115'200 bits/sec. The argument is a
 * divisor of that number, hence the resulting speed becomes (115'200 /
 * divisor) bits/sec.
 */
static void
serial_configure_baud_rate(uint16_t com_port, uint16_t divisor)
{
    x86_port_write_8(SERIAL_LINE_CONTROL_REG(com_port),
                     SERIAL_LINE_ENABLE_DLAB);
    x86_port_write_8(SERIAL_DATA_REG(com_port), (divisor >> 8U) & 0x00ffU);
    x86_port_write_8(SERIAL_DATA_REG(com_port), divisor & 0x00ffU);
    // TODO: OSDev Wiki instructs to "clear the most signficant bit of the Line
    // Control Register" here -- what does this mean?
}

typedef struct [[gnu::packed]]
{
    uint8_t data_length : 2;
    uint8_t stop_bits : 1;
    uint8_t parity_bits : 3;
    uint8_t enable_break_control : 1;
    uint8_t enable_dlab : 1;
} serial_line_configuration_t;

/**
 * Configures how data should be sent over the line for a given COM port.
 *
 * The following table and legend describes the structure of the bits to
 * configure the line: [1][2]
 *
 * Table:
 *   .__________.___.___._______.___._____.
 *   | Bit      | 7 | 6 | 5 4 3 | 2 | 1 0 |
 *   | Content  | d | b | prty  | s | dl  |
 *   | Value    | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
 *   '----------'---'---'-------'---'-----'
 *
 * Legend:
 *   - d: Enables (1) or disables (0) DLAB
 *   - b: Enables (1) or disables (0) break control
 *   - prty: The number of parity bits to use
 *   - s: The number of stop bits to use (0 means 1 bit, 1 means 1.5 or 2)
 *   - dl: Describes the length of the data (0 means 5 bits, 3 means 8 bits)
 *
 * [1]: https://littleosbook.github.io/#configuring-the-line
 * [2]: https://wiki.osdev.org/Serial_Ports#Line_Control_Register
 */
static void
serial_configure_line(uint16_t com_port, serial_line_configuration_t config)
{
    x86_port_write_8(SERIAL_LINE_CONTROL_REG(com_port), *(uint8_t *)&config);
}

typedef struct [[gnu::packed]]
{
    uint8_t enable_fifo_buffer : 1;
    uint8_t clear_receiver_fifo : 1;
    uint8_t clear_transmission_fifo : 1;
    uint8_t dma_mode_select : 1;
    uint8_t reserved : 2;
    uint8_t interrupt_trigger_level : 2;
} serial_fifo_configuration_t;

/**
 * Configures the buffer for a given COM port.
 *
 * The following table and legend describes the structure of the bits to
 * configure the FIFO buffer: [1][2]
 *
 * Table:
 *   .__________.______.____.___._____._____._____.___.
 *   | Bit      | 7  6 | 5  | 4 | 3   | 2   | 1   | 0 |
 *   | Content  | lvl  | bs | r | dma | clt | clr | e |
 *   | Value    | 1  1 | 0  | 0 | 0   | 1   | 1   | 1 | = 0xc7
 *   '----------'------'----'---'-----'-----'-----'---'
 *
 * Legend:
 *   - lvl: How many bytes should be stored in the FIFO buffers (see [3] for
 *          more information)
 *   - bs:  If the buffers should be 16 or 64 bytes large
 *   - r:   Reserved for future use
 *   - dma: How the serial port data should be accessed
 *   - clt: Clear the tranmission FIFO buffer
 *   - clr: Clear the receiver FIFO buffer
 *   - e:   If the FIFO buffer should be enabled or not
 *
 * [1]: https://littleosbook.github.io/#configuring-the-buffers
 * [2]: https://wiki.osdev.org/Serial_Ports#First_In_First_Out_Control_Register
 * [3]: https://wiki.osdev.org/Serial_Ports#Interrupt_Trigger_Level
 */
static void
serial_configure_fifo(uint16_t com_port, serial_fifo_configuration_t config)
{
    x86_port_write_8(SERIAL_FIFO_CONTROL_REG(com_port), *(uint8_t *)&config);
}

typedef struct [[gnu::packed]]
{
    uint8_t data_terminal_ready : 1;
    uint8_t request_to_send : 1;
    uint8_t auxiliary_out_1 : 1;
    uint8_t auxiliary_out_2 : 1;
    uint8_t enable_loopback : 1;
    uint8_t enable_autoflow_control : 1;
    uint8_t reserved : 2;
} serial_modem_configuration_t;

/**
 * Configures the hardware control flow for a given COM port.
 *
 * The following table and legend describes the structure of the bits to
 * configure the modem: [1][2]
 *
 * Table:
 *   .__________.___.___.____.____._____._____._____._____.
 *   | Bit      | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
 *   | Content  | r | r | af | lb | ao2 | ao1 | rts | dtr |
 *   | Value    | 0 | 0 | 0  | 0  | 1   | 0   | 1   | 1   | = 0x0b
 *   '----------'---'---'----'----'-----'-----'-----'-----'
 *
 * Legend:
 *   - r:   Reserved
 *   - af:  Autoflow control enabled
 *   - lb:  Loopback mode (for debugging serial ports)
 *   - ao2: Auxiliary output 2 (used for receiving interrupts)
 *   - ao1: Auxiliary output 1
 *   - rts: Request To Send
 *   - dtr: Data Terminal Ready
 *
 * [1]: https://littleosbook.github.io/#configuring-the-modem
 * [2]: https://wiki.osdev.org/Serial_Ports#Modem_Control_Register
 */
static void
serial_configure_modem(uint16_t com_port, serial_modem_configuration_t config)
{
    x86_port_write_8(SERIAL_MODEM_CONTROL_REG(com_port), *(uint8_t *)&config);
}

/**
 * Tests the given COM port for faultiness.
 */
static enum serial_init_response
serial_test_port(uint16_t com_port)
{
    serial_modem_configuration_t config = {0, 0, 0, 0, 0, 0, 0};

    // Set in loopback mode to test the serial chip
    config.data_terminal_ready = 0b0; // Not yet ready
    config.request_to_send = 0b1;
    config.auxiliary_out_1 = 0b1;
    config.auxiliary_out_2 = 0b1;
    config.enable_loopback = 0b1; // Enable loopback
    serial_configure_modem(com_port, config);

    // Send SERIAL_TEST_BYTE to the port, and check if the serial reponds with
    // the exact same byte.
    x86_port_write_8(SERIAL_DATA_REG(com_port), SERIAL_TEST_BYTE);
    if (x86_port_read_8(SERIAL_DATA_REG(com_port)) != SERIAL_TEST_BYTE)
    {
        return SERIAL_INIT_FAILURE;
    }

    // If serial is not faulty, set it back to normal operation mode
    config.data_terminal_ready = 0b1; // Now ready
    config.request_to_send = 0b1;
    config.auxiliary_out_1 = 0b1;
    config.auxiliary_out_2 = 0b1;
    config.enable_loopback = 0b0; // Disable loopback
    serial_configure_modem(com_port, config);

    // The test has passed
    return SERIAL_INIT_SUCCESS;
}

/**
 * Is the transmit FIFO queue empty for the given serial port?
 */
static bool
serial_is_transmit_fifo_empty(uint16_t com_port)
{
    uint8_t value = x86_port_read_8(com_port + 5) & 0x20U;
    return (bool)(value != 0U);
}

enum serial_init_response
serial_init(uint16_t com_port)
{
    // Disable all interrupts
    // NOTE: The example on OSDev Wiki writes to SERIAL_DATA_REG(com_port) + 1
    x86_port_write_8(SERIAL_DATA_REG(com_port), 0x00U);

    // Set baud rate to 38'400
    serial_configure_baud_rate(com_port, 0x03U);

    // 8 bits, no parity, one stop bit
    serial_line_configuration_t line_config = {
        .data_length = 0b11, // 8 bits
        .stop_bits = 0b0,
        .parity_bits = 0b000,
        .enable_break_control = 0b0,
        .enable_dlab = 0b0,
    };
    serial_configure_line(com_port, line_config);

    // Enable FIFO, clear them, 14 bytes as the size of the queue
    serial_fifo_configuration_t fifo_config = {
        .enable_fifo_buffer = 0b1,
        .clear_receiver_fifo = 0b1,
        .clear_transmission_fifo = 0b1,
        .dma_mode_select = 0b0,
        .reserved = 0b00,
        .interrupt_trigger_level = 0b11, // 14 bytes
    };
    serial_configure_fifo(com_port, fifo_config);

    // IRQs enabled, RTS/DSR set
    // NOTE: Although we've enabled receiving interrupts, we don't have any
    // means to handle incoming data currently.
    serial_modem_configuration_t modem_config = {
        .data_terminal_ready = 0b1,
        .request_to_send = 0b1,
        .auxiliary_out_1 = 0b0,
        .auxiliary_out_2 = 0b1,
        .enable_loopback = 0b0,
        .enable_autoflow_control = 0b0,
        .reserved = 0b00,
    };
    serial_configure_modem(com_port, modem_config);

    // Test port before using it
    return serial_test_port(com_port);
}

void
serial_write(uint16_t com_port, const char *str)
{
    // WARN: Assumes the provided string slice is encoded in UTF-8
    for (const char *ch = str; *ch != '\0'; ch++)
    {
        // Wait for queue to be free
        while (!serial_is_transmit_fifo_empty(com_port))
        {
            asm volatile("pause");
        }
        x86_port_write_8(SERIAL_DATA_REG(com_port), *ch);
    }
}
