#pragma once

#ifndef USING_LIB_GLOBALLY
#define USING_LIB_GLOBALLY 1
#endif

namespace lib::integers
{
	using U8 = unsigned char;
	using U16 = unsigned short;
	using U32 = unsigned int;
	using U64 = unsigned long long;

	using S8 = signed char;
	using S16 = signed short;
	using S32 = signed int;
	using S64 = signed long long;
} // namespace lib::integers

#if USING_LIB_GLOBALLY
using lib::integers::U16;
using lib::integers::U32;
using lib::integers::U64;
using lib::integers::U8;

using lib::integers::S16;
using lib::integers::S32;
using lib::integers::S64;
using lib::integers::S8;
#endif
