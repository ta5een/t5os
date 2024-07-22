#pragma once

void
arch_cpu_init();

// TODO: Inline this function once build system is integrated
[[noreturn]]
void
arch_cpu_halt();

// TODO: Inline this function once build system is integrated
[[noreturn]]
void
arch_cpu_idle_loop();
