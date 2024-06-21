#pragma once

#include <lib/integers.hpp>

namespace kernel
{

[[noreturn]] void _panic(
    const char *msg,
    const char *file,
    usize line,
    const char *function
);

} // namespace kernel

#define KPANIC(msg)                                                            \
    /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */                     \
    do                                                                         \
    {                                                                          \
        kernel::_panic(#msg, __FILE__, __LINE__, __PRETTY_FUNCTION__);         \
    } while (0)
