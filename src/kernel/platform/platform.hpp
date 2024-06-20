#pragma once

#ifdef __i386__
#    define WYOOS_ARCH_IS_I386() 1
#else
#    define WYOOS_ARCH_IS_I386() 0
#endif

#ifdef __x86_64__
#    define WYOOS_ARCH_IS_X86_64() 1
#else
#    define WYOOS_ARCH_IS_X86_64() 0
#endif

#define ARCH(arch) (WYOOS_ARCH_IS_##arch())
