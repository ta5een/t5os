#pragma once

#include <lib/core.hpp>
#include <lib/integers.hpp>

namespace drivers
{

using VgaColorCode = u8;
using VgaScreenChar = u16;

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

class VgaWriter
{
    LIB_MAKE_NONCOPYABLE(VgaWriter)
    LIB_MAKE_NONMOVABLE(VgaWriter)

  public:
    VgaWriter() = default;
    ~VgaWriter() = default;

    void clear_screen();
    void new_line();
    void put_byte(const u8 byte);
    void put_string(const char *str);

  private:
    usize m_col_pos = 0;
    usize m_row_pos = 0;

    static VgaScreenChar create_screen_char(u8 byte, VgaColor fg, VgaColor bg);
    static void overwrite_row_with_blank_screen_chars(usize row);
    inline void set_position(usize col, usize row);
};

// TODO: Wrap this in a mutex to ensure thread-safety
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern VgaWriter WRITER;

} // namespace drivers
