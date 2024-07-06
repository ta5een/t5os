#include "arch/x86/gdt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void(*multiboot_t);

void kmain(
    __attribute__((unused)) multiboot_t multiboot,
    __attribute__((unused)) uint32_t magic
)
{
    gdt_init();
    gdt_load();

    // Idle loop
    while (true)
    {
        // Use the hlt instruction to put the CPU in a low-power state when
        // idle. This reduces CPU usage and power consumption compared to a
        // busy-wait loop.
        // NOLINTNEXTLINE(hicpp-no-assembler)
        asm volatile("hlt");
    }
}
