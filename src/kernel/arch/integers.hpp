#pragma once

#include <kernel/arch/platform.hpp>

namespace kernel::arch::integers
{
	using U8 = unsigned char;
	using U16 = unsigned short;
	using U32 = unsigned int;
	using U64 = unsigned long long;

	using S8 = signed char;
	using S16 = signed short;
	using S32 = signed int;
	using S64 = signed long long;
} // namespace kernel::arch::integers

#if KERNEL_ARCH_GLOBAL_IMPORT
using kernel::arch::integers::U16;
using kernel::arch::integers::U32;
using kernel::arch::integers::U64;
using kernel::arch::integers::U8;

using kernel::arch::integers::S16;
using kernel::arch::integers::S32;
using kernel::arch::integers::S64;
using kernel::arch::integers::S8;
#endif
