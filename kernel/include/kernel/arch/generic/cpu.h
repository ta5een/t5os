#pragma once

#if defined(__i386__) || defined(__x86_64__)
    #include "kernel/arch/x86/cpu.h" // IWYU pragma: export
#endif

/**
 * Initializes the Boostrap Processor (BSP).
 *
 * Since this OS only supports one core at the moment, this is the only
 * initialization process required.
 */
void
cpu_init(void);
