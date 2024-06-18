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

struct VgaWriter
{
  public:
    VgaWriter();
    void new_line();
    void write_byte(const u8 byte);
    void write_string(const char *str);

  private:
    usize _column_pos;
    usize row_col_buffer_index(usize row, usize col) const;
    VgaScreenChar create_screen_char(const char byte, VgaColor fg, VgaColor bg);
    void clear_row(usize row);
};

} // namespace drivers
