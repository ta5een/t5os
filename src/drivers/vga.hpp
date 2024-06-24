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
    LightGreen = 0xa,
    LightCyan = 0xb,
    LightRed = 0xc,
    Pink = 0xd,
    Yellow = 0xe,
    White = 0xf,
};

class VgaWriter
{
    LIB_MAKE_NONCOPYABLE(VgaWriter)
    LIB_MAKE_NONMOVABLE(VgaWriter)

  public:
    VgaWriter() = default;
    ~VgaWriter() = default;

    void clear_screen() const;
    void new_line() const;
    void put_byte(u8 byte) const;
    void put_string(const char *str) const;
    void put_integer(ssize integer) const;
    void put_integer_with_radix(ssize integer, u8 radix) const;

  private:
    mutable usize m_col_pos{0};
    mutable usize m_row_pos{0};

    [[nodiscard]] inline bool try_set_position(usize col, usize row) const;
    inline void unsafely_set_position(usize col, usize row) const;
};

// TODO: Wrap this in a mutex to ensure thread-safety
extern inline const VgaWriter WRITER{};

} // namespace drivers
