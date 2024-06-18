#include <drivers/vga.hpp>

namespace drivers
{

constexpr usize BUFFER_HEIGHT = 25;
constexpr usize BUFFER_WIDTH = 80;
volatile VgaScreenChar *const VGA_MEMORY = (volatile VgaScreenChar *)0xB8000;

VgaColor const DEFAULT_FG = VgaColor::Green;
VgaColor const DEFAULT_BG = VgaColor::Black;

VgaWriter::VgaWriter()
    : _column_pos(0)
{
}

void VgaWriter::new_line()
{
    for (usize row = 1; row < BUFFER_HEIGHT; row++)
    {
        for (usize col = 0; col < BUFFER_WIDTH; col++)
        {
            auto curr_index = row_col_buffer_index(row, col);
            auto new_index = row_col_buffer_index(row - 1, col);
            auto character = VGA_MEMORY[curr_index];
            VGA_MEMORY[new_index] = character;
        }
    }

    clear_row(BUFFER_HEIGHT - 1);
    _column_pos = 0;
}

void VgaWriter::write_byte(const u8 byte)
{
    if (byte == '\n')
    {
        new_line();
    }
    else
    {
        if (_column_pos >= BUFFER_WIDTH)
        {
            new_line();
        }

        auto index = row_col_buffer_index(BUFFER_HEIGHT - 1, _column_pos);
        auto character = create_screen_char(byte, DEFAULT_FG, DEFAULT_BG);
        VGA_MEMORY[index] = character;
        _column_pos += 1;
    }
}

void VgaWriter::write_string(const char *str)
{
    for (usize i = 0; str[i] != '\0'; i++)
    {
        u8 byte = str[i];
        if (byte == '\n' || (byte >= 0x20 && byte <= 0x7E))
        {
            write_byte(byte);
        }
        else
        {
            write_byte(0xFE);
        }
    }
}

usize VgaWriter::row_col_buffer_index(usize row, usize col) const
{
    return (row * BUFFER_WIDTH) + col;
}

VgaScreenChar VgaWriter::create_screen_char(
    const char byte,
    VgaColor fg,
    VgaColor bg
)
{
    auto color_code = ((u8)fg) | ((u8)bg << 4);
    return ((u16)byte | ((u16)color_code << 8));
}

void VgaWriter::clear_row(usize row)
{
    for (usize col = 0; col < BUFFER_WIDTH; col++)
    {
        auto blank = create_screen_char('\0', DEFAULT_FG, DEFAULT_BG);
        auto index = row_col_buffer_index(row, col);
        VGA_MEMORY[index] = blank;
    }
}

} // namespace drivers
