#pragma once

#ifndef USING_LIB_GLOBALLY
#    define USING_LIB_GLOBALLY 1
#endif

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
} // namespace lib

#if USING_LIB_GLOBALLY
using lib::u16;
using lib::u32;
using lib::u64;
using lib::u8;

using lib::s16;
using lib::s32;
using lib::s64;
using lib::s8;
#endif
