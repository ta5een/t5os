#pragma once

void
arch_cpu_init();

[[noreturn]]
static inline void
arch_cpu_halt()
{
    while (true)
    {
        asm volatile("cli; hlt");
    }
}

[[noreturn]]
static inline void
arch_cpu_idle_loop()
{
    while (true)
    {
        // Use the hlt instruction to put the CPU in a low-power state when
        // idle. This reduces CPU usage and power consumption compared to a
        // busy-wait loop.
        asm volatile("hlt");
    }
}
