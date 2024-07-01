#pragma once

#include <lib/core.hpp>
#include <lib/integers.hpp>

namespace kernel
{

[[noreturn]]
void _on_assert(const char *msg, const char *location, const char *function);

} // namespace kernel

#define KASSERT(expr)                                                          \
    /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */                     \
    do                                                                         \
    {                                                                          \
        if (!static_cast<bool>(expr)) [[unlikely]]                             \
        {                                                                      \
            kernel::_on_assert(                                                \
                #expr,                                                         \
                __FILE__ ":" LIB_STRINGIFY(__LINE__),                          \
                __PRETTY_FUNCTION__                                            \
            );                                                                 \
        }                                                                      \
    } while (0)

#ifdef DEBUG_KERNEL
#    define DEBUG_KASSERT(expr)                                                \
        /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */                 \
        do                                                                     \
        {                                                                      \
            if (!static_cast<bool>(expr)) [[unlikely]]                         \
            {                                                                  \
                kernel::_on_assert(                                            \
                    #expr,                                                     \
                    __FILE__ ":" LIB_STRINGIFY(__LINE__),                      \
                    __PRETTY_FUNCTION__                                        \
                );                                                             \
            }                                                                  \
        } while (0)
#else
#    define DEBUG_KASSERT(expr) /* no-op */
#endif
