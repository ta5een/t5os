#include "kernel/arch/x86/devices/vga.h"
#include <stddef.h>
#include <stdint.h>

#define ASCII_CHAR_UNKNOWN        0xfeU // U+25a0 BLACK SQUARE (Code Page 437)
#define ASCII_PRINTABLE_RANGE_MIN 0x20U // U+0020 SPACE
#define ASCII_PRINTABLE_RANGE_MAX 0x7eU // U+007e TILDE

#define VGA_DEFAULT_FG ((enum vga_color)VGA_COLOR_LIGHT_GRAY)
#define VGA_DEFAULT_BG ((enum vga_color)VGA_COLOR_BLACK)

#define VGA_CHAR(byte, fg, bg)                                                 \
    ({                                                                         \
        uint8_t color_code = ((uint8_t)fg) | (uint8_t)((uint8_t)bg << 4U);     \
        ((uint16_t)byte) | (uint16_t)(color_code << 8U);                       \
    })
#define VGA_BLANK_CHAR VGA_CHAR('\0', VGA_DEFAULT_FG, VGA_DEFAULT_BG)

enum vga_color : uint8_t
{
    VGA_COLOR_BLACK = 0x0,
    VGA_COLOR_BLUE = 0x1,
    VGA_COLOR_GREEN = 0x2,
    VGA_COLOR_CYAN = 0x3,
    VGA_COLOR_RED = 0x4,
    VGA_COLOR_MAGENTA = 0x5,
    VGA_COLOR_BROWN = 0x6,
    VGA_COLOR_LIGHT_GRAY = 0x7,
    VGA_COLOR_DARK_GRAY = 0x8,
    VGA_COLOR_LIGHT_BLUE = 0x9,
    VGA_COLOR_LIGHT_GREEN = 0xa,
    VGA_COLOR_LIGHT_CYAN = 0xb,
    VGA_COLOR_LIGHT_RED = 0xc,
    VGA_COLOR_PINK = 0xd,
    VGA_COLOR_YELLOW = 0xe,
    VGA_COLOR_WHITE = 0xf,
};

struct vga
{
    /**
     * A region of memory available representing a two-dimensional array of
     * VGA_WIDTH columns and VGA_HEIGHT rows.
     */
    volatile vga_char_t *buffer;
    /**
     * The current x position. This is zero-indexed.
     */
    size_t col_pos;
    /**
     * The current y position. This is zero-indexed.
     */
    size_t row_pos;
};

struct vga *
vga_get()
{
    // TODO: Wrap accesses to this struct in a mutex
    static struct vga s_vga = {.buffer = NULL, .col_pos = 0, .row_pos = 0};
    return &s_vga;
}

void
vga_init(struct vga *vga, volatile vga_char_t *buffer)
{
    vga->buffer = buffer;
    vga->col_pos = 0;
    vga->row_pos = 0;
}

/**
 * Calculates the appropriate offset from the start of the buffer given a column
 * and row indices.
 */
static inline size_t
vga_buffer_index_at(size_t col, size_t row)
{
    // TODO: Add asserts
    return (row * VGA_WIDTH) + col;
}

/**
 * Reads the VGA character from the buffer at the provided column and row
 * indices.
 *
 * Don't assume the data returned represents a "valid" VGA character.
 */
static vga_char_t
vga_buffer_read(const volatile vga_char_t *buffer, size_t col, size_t row)
{
    size_t buffer_idx = vga_buffer_index_at(col, row);
    return buffer[buffer_idx];
}

/**
 * Writes the provided VGA character into the buffer at the provided column and
 * row indices.
 */
static void
vga_buffer_write(volatile vga_char_t *buffer, vga_char_t vch, size_t col,
                 size_t row)
{
    size_t buffer_idx = vga_buffer_index_at(col, row);
    // If both the new char and the current char at this position are the same,
    // we can skip the write
    if (buffer[buffer_idx] != vch)
    {
        buffer[buffer_idx] = vch;
    }
}

/**
 * Directly manipulate the VGA writer position.
 *
 * This function will NOT check if the provided column and row indices lie
 * within the bounds of the VGA buffer. Use `vga_can_set_pos` and
 * `vga_try_set_pos` for that.
 */
static inline void
vga_unsafely_set_pos(struct vga *vga, size_t col, size_t row)
{
    vga->col_pos = col;
    vga->row_pos = row;
}

/**
 * Does the provided column and row indices lie within the bounds of the VGA
 * buffer?
 */
[[nodiscard]]
static inline bool
vga_can_set_pos(size_t col, size_t row)
{
    // NOLINTNEXTLINE(readability-implicit-bool-conversion)
    return (col < VGA_WIDTH) && (row < VGA_HEIGHT);
}

/**
 * Attemps to set the VGA writer position to the provided column and row
 * indices.
 *
 * Returns true if this operation succeeded, otherwise false.
 */
[[nodiscard]]
static inline bool
vga_try_set_pos(struct vga *vga, size_t col, size_t row)
{
    if (vga_can_set_pos(col, row))
    {
        vga_unsafely_set_pos(vga, col, row);
        return true;
    }

    return false;
}

/**
 * Clears the contents of the provided row.
 */
static void
vga_clear_row(struct vga *vga, size_t row)
{
    for (size_t col = 0; col < VGA_WIDTH; col++)
    {
        // WARN: `row` is not checked, would this be a problem?
        vga_buffer_write(vga->buffer, VGA_BLANK_CHAR, col, row);
    }
}

/**
 * Moves the VGA writer position to the next line, scrolling the contents
 * upwards if necessary.
 */
static void
vga_new_line(struct vga *vga)
{
    // If we can't simply increment the y position because we're at the bottom
    // of the screen, we'll scroll the buffer contents upwards and then set the
    // y position to the last line
    if (!vga_try_set_pos(vga, 0, vga->row_pos + 1))
    {
        // Scroll buffer contents upwards by traversing into each row (starting
        // at the second row) and moving each character to the row above
        for (size_t row = 1; row < VGA_HEIGHT; row++)
        {
            for (size_t col = 0; col < VGA_WIDTH; col++)
            {
                vga_char_t vch = vga_buffer_read(vga->buffer, col, row);
                vga_buffer_write(vga->buffer, vch, col, row - 1);
            }
        }

        // Overwrite old contents of the last line with a fresh blank row
        vga_clear_row(vga, VGA_HEIGHT - 1);
        // Reset x position, cap y position to the last line
        vga_unsafely_set_pos(vga, 0, VGA_HEIGHT - 1);
    }
}

/**
 * Writes a single byte to screen.
 *
 * Currently, only the line feed character ('\n') is treated specially. All
 * other control characters and non-printable characters will be replaced with
 * ASCII_CHAR_UNKNOWN.
 */
static void
vga_put_byte(struct vga *vga, uint8_t byte)
{
    if (byte == '\n')
    {
        vga_new_line(vga);
    }
    else
    {
        // Replace the character if it isn't "printable"
        if (!(byte >= ASCII_PRINTABLE_RANGE_MIN &&
              byte <= ASCII_PRINTABLE_RANGE_MAX))
        {
            byte = ASCII_CHAR_UNKNOWN;
        }

        // WARN: `col` and `row` is not checked, would this be a problem?
        vga_char_t vch = VGA_CHAR(byte, VGA_DEFAULT_FG, VGA_DEFAULT_BG);
        vga_buffer_write(vga->buffer, vch, vga->col_pos, vga->row_pos);
        // If we can't simply increment the x position because we're at the edge
        // of the screen, we'll move to the next line (scrolling the existing
        // contents upwards if necessary)
        if (!vga_try_set_pos(vga, vga->col_pos + 1, vga->row_pos))
        {
            vga_new_line(vga);
        }
    }
}

void
vga_clear_screen(struct vga *vga)
{
    for (size_t row = 0; row < VGA_HEIGHT; row++)
    {
        vga_clear_row(vga, row);
    }

    // Reset position
    vga_unsafely_set_pos(vga, 0, 0);
}

void
vga_print(struct vga *vga, const char *str)
{
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        vga_put_byte(vga, str[i]);
    }
}

void
vga_println(struct vga *vga, const char *str)
{
    vga_print(vga, str);
    vga_new_line(vga);
}
