#pragma once

#include <lib/core.hpp>
#include <lib/integers.hpp>

namespace kernel
{

class Port
{
    LIB_MAKE_NONCOPYABLE(Port)
    LIB_MAKE_NONMOVABLE(Port)

  protected:
    explicit Port(u16 port_number);
    ~Port() = default;

    u16 m_port_number{0};
};

class Port8Bit : public Port
{
    LIB_MAKE_NONCOPYABLE(Port8Bit)
    LIB_MAKE_NONMOVABLE(Port8Bit)

  public:
    explicit Port8Bit(u16 port_number);
    ~Port8Bit() = default;

    virtual void write(u8 data);
    virtual u8 read();
};

class Port8BitSlow : public Port8Bit
{
    LIB_MAKE_NONCOPYABLE(Port8BitSlow)
    LIB_MAKE_NONMOVABLE(Port8BitSlow)

  public:
    explicit Port8BitSlow(u16 port_number);
    ~Port8BitSlow() = default;

    void write(u8 data) override;
};

class Port16Bit : public Port
{
    LIB_MAKE_NONCOPYABLE(Port16Bit)
    LIB_MAKE_NONMOVABLE(Port16Bit)

  public:
    explicit Port16Bit(u16 port_number);
    ~Port16Bit() = default;

    virtual void write(u16 data);
    virtual u16 read();
};

class Port32Bit : public Port
{
    LIB_MAKE_NONCOPYABLE(Port32Bit)
    LIB_MAKE_NONMOVABLE(Port32Bit)

  public:
    explicit Port32Bit(u16 port_number);
    ~Port32Bit() = default;

    virtual void write(u32 data);
    virtual u32 read();
};

} // namespace kernel
