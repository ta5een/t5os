#include "kernel/arch/x86/cpu.h"
#include "kernel/arch/x86/32/idt.h"
#include "kernel/arch/x86/32/isr.h"
#include "kernel/arch/x86/devices/serial.h"
#include "kernel/arch/x86/devices/vga.h"
#include "kernel/arch/x86/gdt.h"

#define VGA_BUFFER_ADDRESS ((volatile vga_char_t *)0xb8000U)

static volatile vga_char_t *s_vga_buffer = VGA_BUFFER_ADDRESS;

void
arch_cpu_init()
{
    // TODO: Move this to arch-generic console_init function
    struct vga *vga = vga_get();
    vga_init(vga, s_vga_buffer);
    vga_clear_screen(vga);
    vga_println(vga, "t5os v0.0.1");

    // TODO: Consider removing this
    vga_print(vga, "Clearing interrupts...");
    asm volatile("cli");
    vga_println(vga, " [DONE]");

    if (serial_init(SERIAL_COM1_BASE) != SERIAL_INIT_SUCCESS)
    {
        // TODO: Do something useful here...
        vga_println(vga, "[ WARN ] Serial connection could not be initialized");
    }

    // Load the GDT for the BSP
    x86_gdt_init();

    // Load the IDT and register the ISRs and IRQs for the BSP
    // TODO: Refactor this to use x86-generic variants
    x86_32_idt_init();
    x86_32_isr_init();
    // TODO: Activate all interrupts with STI once IRQs are implemented
    // x86_32_idt_activate();

    // TODO: Remove this
    // Test software interrupts
    vga_print(vga, "Triggering INT 0x02...");
    asm volatile("int $0x02");
    vga_println(vga, " [DONE]");

    // TODO: Remove this
    vga_print(vga, "Triggering INT 0x03...");
    asm volatile("int $0x03");
    vga_println(vga, " [DONE]");

    // TODO: Remove this
    vga_print(vga, "Triggering INT 0x04...");
    asm volatile("int $0x04");
    vga_println(vga, " [DONE]");

    // TODO: Remove this
    vga_print(vga, "Triggering INT 0x1f...");
    asm volatile("int $0x1f");
    vga_println(vga, " [DONE]");
}

void
arch_cpu_halt()
{
    while (true)
    {
        asm volatile("cli; hlt");
    }
}

void
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
