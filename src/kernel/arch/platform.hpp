#pragma once

#ifdef __i386__
#define KERNEL_ARCH_IS_I386() 1
#else
#define KERNEL_ARCH_IS_I386() 0
#endif

#if KERNEL_ARCH_IS_I386()
#endif
