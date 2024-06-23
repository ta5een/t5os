#include <drivers/vga.hpp>
#include <kernel/kassert.hpp>
#include <lib/integers.hpp>

namespace drivers
{

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
const auto VGA_BUFFER = reinterpret_cast<volatile VgaScreenChar *>(0xb8000);

const usize BUFFER_WIDTH = 80;
const usize BUFFER_HEIGHT = 25;
const VgaColor DEFAULT_FG = VgaColor::Green;
const VgaColor DEFAULT_BG = VgaColor::Black;

const u8 ITOA_DEFAULT_RADIX = 10;
const u8 ITOA_MAX_STR_LEN = 255;
const u8 ITOA_RADIX_MIN = 2;
const u8 ITOA_RADIX_MAX = 36;
const char *const ITOA_SEARCH_STR =
    "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";

// Initialize global variable with default constructor
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
VgaWriter WRITER;

class VgaMemoryBuffer
{
  public:
    static usize index_at(usize col, usize row)
    {
        // WARN: Ensure the implementation of VgaWriter always provides correct
        // column and row indices, otherwise when KASSERT attempts to print to
        // VGA text mode with an invalid index, an infinite loop will occur.
        KASSERT(col < BUFFER_WIDTH && "invalid column index");
        KASSERT(row < BUFFER_HEIGHT && "invalid row index");
        return (row * BUFFER_WIDTH) + col;
    }

    static VgaScreenChar read(usize col, usize row)
    {
        auto buffer_index = index_at(col, row);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return VGA_BUFFER[buffer_index];
    }

    static void write(VgaScreenChar screen_char, usize col, usize row)
    {
        auto buffer_index = index_at(col, row);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        VGA_BUFFER[buffer_index] = screen_char;
    }
};

void VgaWriter::clear_screen()
{
    for (usize row = 0; row < BUFFER_HEIGHT; row++)
    {
        overwrite_row_with_blank_screen_chars(row);
    }

    // Reset position
    unsafely_set_position(0, 0);
}

void VgaWriter::new_line()
{
    // If we can't simply increment the y position because we're at the bottom
    // of the screen, we'll scroll the buffer contents upwards and then set the
    // y position to the last line
    if (!try_set_position(0, m_row_pos + 1))
    {
        // Scroll buffer contents upwards
        for (usize col = 0; col < BUFFER_WIDTH; col++)
        {
            for (usize row = 1; row < BUFFER_HEIGHT; row++)
            {
                auto character = VgaMemoryBuffer::read(col, row);
                VgaMemoryBuffer::write(character, col, row - 1);
            }
        }

        // Reset x position, cap y position to the last line
        unsafely_set_position(0, BUFFER_HEIGHT - 1);
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
        // If we can't simply increment the x position because we're at the
        // edge of the screen, we'll move to the next line (scrolling up if
        // necessary)
        if (!try_set_position(m_col_pos + 1, m_row_pos))
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
        if (byte == '\n' || (byte >= 0x20 && byte <= 0x7e))
        {
            put_byte(byte);
        }
        else
        {
            put_byte(0xfe);
        }
    }
}

void VgaWriter::put_integer(const ssize integer)
{
    put_integer_with_radix(integer, ITOA_DEFAULT_RADIX);
}

// https://www.strudel.org.uk/itoa/
// https://wiki.osdev.org/Printing_To_Screen#Printing_Integers
void VgaWriter::put_integer_with_radix(const ssize integer, const u8 radix)
{
    KASSERT(radix >= ITOA_RADIX_MIN && "provided radix is too small");
    KASSERT(radix <= ITOA_RADIX_MAX && "provided radix is too large");

    // TODO: Once dynamic memory management is implemented, it might be
    // preferrable to allocate this new string in the heap.
    char str[ITOA_MAX_STR_LEN]{};
    usize idx = 0;
    ssize value = integer;

    // Convert each digit to a character, from the least significant digit to
    // the most significant (i.e. in reverse order).
    do
    {
        // ITOA_SEARCH_STR is a palindrome, such that a negative digit will
        // produce the same character as a positive digit. This trick makes
        // abs() unnecessary.
        str[idx] = ITOA_SEARCH_STR[(ITOA_RADIX_MAX - 1) + value % radix];
        idx += 1;
        value /= radix; // this will truncate (discard) the fractional part
    } while (value != 0);

    // Append '-' for negative numbers to the end of the reversed string
    if (integer < 0)
    {
        str[idx] = '-';
        idx += 1;
    }

    // Ensure the string is terminated
    str[idx] = '\0';
    idx -= 1;

    // Flip string to the correct order
    usize rev = 0;
    while (rev < idx)
    {
        char tmp = str[idx];
        str[idx] = str[rev];
        str[rev] = tmp;
        rev += 1;
        idx -= 1;
    }

    put_string(str);
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

bool VgaWriter::can_set_position(usize col, usize row)
{
    return (col < BUFFER_WIDTH) && (row < BUFFER_HEIGHT);
}

inline bool VgaWriter::try_set_position(usize col, usize row)
{
    if (can_set_position(col, row))
    {
        unsafely_set_position(col, row);
        return true;
    }
    else
    {
        return false;
    }
}

inline void VgaWriter::unsafely_set_position(usize col, usize row)
{
    m_col_pos = col;
    m_row_pos = row;
}

} // namespace drivers
