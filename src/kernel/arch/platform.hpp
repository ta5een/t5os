#pragma once

#if defined(__i386__) && !defined(__x86_64__)
#    define T5OS_ARCH_IS_X86_32() 1
#else
#    define T5OS_ARCH_IS_X86_32() 0
#endif

#ifdef __x86_64__
#    define T5OS_ARCH_IS_X86_64() 1
#else
#    define T5OS_ARCH_IS_X86_64() 0
#endif

#define ARCH(arch) (T5OS_ARCH_IS_##arch())
