#include <arch/cpu.h>
#include <stdint.h>

typedef void(*multiboot_t);

void
kmain(multiboot_t /*multiboot*/, uint32_t /*magic*/)
{
    // Initialize architecture-specific components
    arch_cpu_init();

    // Nothing to do right now, go into an idle loop
    arch_cpu_idle_loop();
}
