#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void(*multiboot_t);

static struct gdt s_gdt;
static struct idt s_idt;

void kmain(
    [[gnu::unused]] multiboot_t /*multiboot*/,
    [[gnu::unused]] uint32_t /*magic*/
)
{
    // Load the GDT for the BSP
    gdt_init(&s_gdt);
    gdt_load(&s_gdt);

    // Load the IDT for the BSP
    idt_init(&s_idt, GDT_IDX_KCODE);
    idt_load(&s_idt);
    idt_activate(&s_idt);

    // Idle loop
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
