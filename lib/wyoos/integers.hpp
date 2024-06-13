#pragma once

#include <wyoos/platform.hpp>

namespace wyoos::integers
{

	using U8 = unsigned char;
	using U16 = unsigned short;
	using U32 = unsigned int;
	using U64 = unsigned long long;

	using S8 = signed char;
	using S16 = signed short;
	using S32 = signed int;
	using S64 = signed long long;
} // namespace wyoos::integers

#if WYOOS_GLOBAL_IMPORT
using wyoos::integers::U16;
using wyoos::integers::U32;
using wyoos::integers::U64;
using wyoos::integers::U8;

using wyoos::integers::S16;
using wyoos::integers::S32;
using wyoos::integers::S64;
using wyoos::integers::S8;
#endif
