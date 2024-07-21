#include <kernel/arch/generic/arch_cpu_init.h>
#include <stdint.h>

typedef void(*multiboot_t);

void
kmain(multiboot_t /*multiboot*/, uint32_t /*magic*/)
{
    // Initialize architecture-specific components
    arch_cpu_init();

    // Idle loop
    // TODO: Move this to generic CPU interface
    while (true)
    {
#if defined(__i386__) || defined(__x86_64__)
        // Use the hlt instruction to put the CPU in a low-power state when
        // idle. This reduces CPU usage and power consumption compared to a
        // busy-wait loop.
        asm volatile("hlt");
#else
    #warning "Idle loop not defined for current architecture"
#endif
    }
}
