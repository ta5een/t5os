#pragma once

#include <lib/core.hpp>
#include <lib/integers.hpp>

namespace kernel
{

[[noreturn]]
void _on_panic(const char *msg, const char *location, const char *function);

} // namespace kernel

#define KPANIC(msg)                                                            \
    /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */                     \
    do                                                                         \
    {                                                                          \
        kernel::_on_panic(                                                     \
            #msg, __FILE__ ":" LIB_STRINGIFY(__LINE__), __PRETTY_FUNCTION__    \
        );                                                                     \
    } while (0)
