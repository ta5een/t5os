#pragma once

/**
 * Stops the CPU from executing further instructions.
 *
 * Any non-maskable interrupts will be cleared before halting the CPU again.
 *
 * This is only needed when you want the CPU to stop completely, such as right
 * after a kernel panic.
 */
[[noreturn]]
static inline void
cpu_halt()
{
    while (true)
    {
        asm volatile("cli; hlt");
    }
}

/**
 * Puts the CPU in a low-power state and wait for interrupts.
 *
 * This is useful when the CPU has nothing else to do and you would like it to
 * stay in an idle, yet responsive, state.
 */
[[noreturn]]
static inline void
cpu_idle_loop()
{
    while (true)
    {
        asm volatile("hlt");
    }
}
