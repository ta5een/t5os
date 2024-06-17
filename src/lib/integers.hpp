#pragma once

#include <lib/core.hpp>

namespace lib
{

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using s8 = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;

// Taken from:
// https://awesomekling.github.io/How-SerenityOS-declares-ssize_t/
using usize = __SIZE_TYPE__;
#define unsigned signed
using ssize = __SIZE_TYPE__;
#undef unsigned

} // namespace lib

#if USING_LIB_GLOBALLY
// clang-format off
using lib::u8;
using lib::u16;
using lib::u32;
using lib::u64;
using lib::usize;
// clang-format on

// clang-format off
using lib::s8;
using lib::s16;
using lib::s32;
using lib::s64;
using lib::ssize;
// clang-format on
#endif
