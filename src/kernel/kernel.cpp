#include <kernel/arch/platform.hpp>
#include <lib/integers.hpp>

using Multiboot = void *;
using Constructor = void (*)();

namespace vga
{

using ColorCode = u8;
using ScreenChar = u16;

constexpr usize BUFFER_HEIGHT = 25;
constexpr usize BUFFER_WIDTH = 80;
volatile ScreenChar *const VGA_MEMORY = (volatile ScreenChar *)0xB8000;

enum class VgaColor : u8
{
    Black = 0x0,
    Blue = 0x1,
    Green = 0x2,
    Cyan = 0x3,
    Red = 0x4,
    Magenta = 0x5,
    Brown = 0x6,
    LightGray = 0x7,
    DarkGray = 0x8,
    LightBlue = 0x9,
    LightGreen = 0xA,
    LightCyan = 0xB,
    LightRed = 0xC,
    Pink = 0xD,
    Yellow = 0xE,
    White = 0xF,
};

VgaColor const DEFAULT_FG = VgaColor::Green;
VgaColor const DEFAULT_BG = VgaColor::Black;

struct VgaWriter
{
  private:
    usize _column_pos;

    auto row_col_buffer_index(usize row, usize col) -> usize
    {
        return (row * BUFFER_WIDTH) + col;
    }

    auto create_screen_char(const char byte, VgaColor fg, VgaColor bg)
        -> ScreenChar
    {
        auto color_code = ((u8)fg) | ((u8)bg << 4);
        return ((u16)byte | ((u16)color_code << 8));
    }

    auto clear_row(usize row) -> void
    {
        for (usize col = 0; col < BUFFER_WIDTH; col++)
        {
            auto blank = create_screen_char('\0', DEFAULT_FG, DEFAULT_BG);
            auto index = row_col_buffer_index(row, col);
            VGA_MEMORY[index] = blank;
        }
    }

  public:
    VgaWriter()
        : _column_pos(0)
    {
    }

    auto new_line() -> void
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
    };

    auto write_byte(const u8 byte) -> void
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

    auto write_string(const char *str) -> void
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
};

} // namespace vga

vga::VgaWriter writer;

extern "C" Constructor start_ctors;
extern "C" Constructor end_ctors;

/**
 * Required run-time function for driving C++ constructors.
 */
extern "C" void call_ctors()
{
    writer.write_string("CALL_CTORS\n");
    for (Constructor *ctor = &start_ctors; ctor != &end_ctors; ctor++)
    {
        writer.write_string("CALL_CTORS_LOOP\n");
        (*ctor)();
    }
}

// TODO: Not sure if this function can replace `call_ctors`
// [[gnu::constructor]] void global_ctors()
// {
//     writer.write_string("GLOBAL_CTORS\n");
// }

/**
 * Entry point of the kernel.
 */
extern "C" void _kmain(Multiboot /* multiboot */, u32 /* magic */)
{
    const char *arch_msg;
    if (KERNEL_ARCH_IS_I386())
    {
        arch_msg = "Architecture: i386";
    }

    writer.write_string("Welcome to WYOOS\n");
    writer.write_string(arch_msg);

    // Infinite loop
    while (true) {}
}
