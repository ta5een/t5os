#include <kernel/arch/x86/gdt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void(*multiboot_t);

static gdt_t s_gdt;

void kmain(
    [[gnu::unused]] multiboot_t /*multiboot*/,
    [[gnu::unused]] uint32_t /*magic*/
)
{
    // Load the GDT for the BSP
    gdt_init(&s_gdt);
    gdt_load(&s_gdt);

    // Idle loop
    while (true)
    {
        // Use the hlt instruction to put the CPU in a low-power state when
        // idle. This reduces CPU usage and power consumption compared to a
        // busy-wait loop.
        asm volatile("hlt");
    }
}
