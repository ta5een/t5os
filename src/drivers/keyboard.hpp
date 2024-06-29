#pragma once

#include <kernel/platform/x86/interrupts.hpp>
#include <kernel/platform/x86/port.hpp>

namespace drivers
{

// Rudimentary support for PS/2 keyboards
class KeyboardDriver : public kernel::InterruptHandler
{
  public:
    KeyboardDriver(kernel::InterruptManager *manager);
    ~KeyboardDriver();

    u32 handle_interrupt(u32 esp) override;

  private:
    kernel::Port8Bit m_data_port;
    kernel::Port8Bit m_command_port;
};

} // namespace drivers
