#pragma once

#include <stdint.h>

#define VGA_WIDTH  ((size_t)80U)
#define VGA_HEIGHT ((size_t)25U)

/**
 * Each item in the VGA buffer encodes an ASCII character, a foreground color,
 * and a background color. These details are packed into two bytes.
 */
typedef uint16_t vga_char_t;

/**
 * A struct that holds internal details about the VGA writer.
 */
struct vga;

/**
 * Retrieves the global instance of the VGA writer.
 */
struct vga *
vga_get();

/**
 * Intializes the VGA writer with the address of the provided buffer.
 *
 * Since there's only one global instance of the VGA writer, it is expected that
 * this function will be called once throughtout the lifetime of the kernel.
 *
 * The provided buffer must have a capacity of VGA_WIDTH * VGA_HEIGHT. The VGA
 * writer will assume the buffer can be represented as a two-dimensional array,
 * where VGA_WIDTH marks the number of columns and VGA_HEIGHT marks the number
 * of rows.
 *
 * It is expected that the provided buffer points to a valid address and is
 * cleared (i.e. contains no garbage data). Additionally, it is assumed that the
 * buffer resides in RAM, such that contents removed from it will be gone
 * forever. Due to its volatile nature, it is inadvisable to directly read and
 * write to this buffer -- doing so will lead to unintended consequences.
 */
void
vga_init(struct vga *vga, volatile vga_char_t *buffer);

/**
 * Clears the contents of the VGA buffer.
 */
void
vga_clear_screen(struct vga *vga);

/**
 * Writes the contents of the provided string to the VGA buffer.
 *
 * TODO: Ability to accept a format string and variable number of arguments.
 */
void
vga_print(struct vga *vga, const char *str);

/**
 * Writes the contents of the provided string to the VGA buffer, moving the
 * cursor to the next line.
 *
 * TODO: Ability to accept a format string and variable number of arguments.
 */
void
vga_println(struct vga *vga, const char *str);
