#include <drivers/vga.hpp>
#include <lib/integers.hpp>

namespace drivers
{

volatile VgaScreenChar *const VGA_MEMORY = (volatile VgaScreenChar *)0xB8000;

constexpr usize const BUFFER_WIDTH = 80;
constexpr usize const BUFFER_HEIGHT = 25;
constexpr VgaColor const DEFAULT_FG = VgaColor::Green;
constexpr VgaColor const DEFAULT_BG = VgaColor::Black;

VgaWriter WRITER = VgaWriter();

class VgaMemoryBuffer
{
  public:
    static usize index_at(usize col, usize row)
    {
        return (row * BUFFER_WIDTH) + col;
    }

    static VgaScreenChar read(usize col, usize row)
    {
        auto buffer_index = index_at(col, row);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return VGA_MEMORY[buffer_index];
    }

    static void write(VgaScreenChar screen_char, usize col, usize row)
    {
        auto buffer_index = index_at(col, row);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        VGA_MEMORY[buffer_index] = screen_char;
    }
};

VgaWriter::VgaWriter()
    : m_col_pos(0)
    , m_row_pos(0)
{
}

void VgaWriter::clear_screen()
{
    for (usize row = 0; row < BUFFER_HEIGHT; row++)
    {
        overwrite_row_with_blank_screen_chars(row);
    }

    // Reset position
    set_position(0, 0);
}

void VgaWriter::new_line()
{
    if (m_row_pos < BUFFER_HEIGHT)
    {
        // Increment y position
        set_position(0, m_row_pos + 1);
    }
    else
    {
        // Scroll up
        for (usize col = 0; col < BUFFER_WIDTH; col++)
        {
            for (usize row = 1; row < BUFFER_HEIGHT; row++)
            {
                auto character = VgaMemoryBuffer::read(col, row);
                VgaMemoryBuffer::write(character, col, row - 1);
            }
        }

        // Reset x position, cap y position to last line
        set_position(0, BUFFER_HEIGHT - 1);
    }
}

void VgaWriter::put_byte(const u8 byte)
{
    if (byte == '\n')
    {
        new_line();
    }
    else
    {
        auto character = create_screen_char(byte, DEFAULT_FG, DEFAULT_BG);
        VgaMemoryBuffer::write(character, m_col_pos, m_row_pos);
        if (m_col_pos < BUFFER_WIDTH)
        {
            // Incremenet x position, keep y position
            set_position(m_col_pos + 1, m_row_pos);
        }
        else
        {
            new_line();
        }
    }
}

void VgaWriter::put_string(const char *str)
{
    for (usize i = 0; str[i] != '\0'; i++)
    {
        u8 byte = str[i];
        if (byte == '\n' || (byte >= 0x20 && byte <= 0x7E))
        {
            put_byte(byte);
        }
        else
        {
            put_byte(0xFE);
        }
    }
}

VgaScreenChar VgaWriter::create_screen_char(u8 byte, VgaColor fg, VgaColor bg)
{
    u8 color_code = ((u8)fg) | (u8)((u8)bg << 4U);
    return ((u16)byte | (u16)((u16)color_code << 8U));
}

void VgaWriter::overwrite_row_with_blank_screen_chars(usize row)
{
    for (usize col = 0; col < BUFFER_WIDTH; col++)
    {
        auto blank = create_screen_char('\0', DEFAULT_FG, DEFAULT_BG);
        VgaMemoryBuffer::write(blank, col, row);
    }
}

void VgaWriter::set_position(usize col, usize row)
{
    m_col_pos = col;
    m_row_pos = row;
}

} // namespace drivers
