#include <kernel/arch/generic/arch_cpu_init.h>
#include <kernel/arch/x86/devices/serial.h>
#include <kernel/arch/x86/devices/vga.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/interrupts.h>

#define VGA_BUFFER_ADDRESS ((volatile vga_char_t *)0xb8000U)

static volatile vga_char_t *s_vga_buffer = VGA_BUFFER_ADDRESS;

void
arch_cpu_init()
{
    struct vga *vga = vga_get();
    vga_init(vga, s_vga_buffer);
    vga_clear_screen(vga);
    vga_println(vga, "t5os v0.0.1");

    vga_print(vga, "Clearing interrupts...");
    asm volatile("cli");
    vga_println(vga, " [DONE]");

    if (serial_init(SERIAL_COM1_BASE) != SERIAL_INIT_SUCCESS)
    {
        // TODO: Do something useful here...
        vga_println(vga, "[ WARN ] Serial connection could not be initialized");
    }

    // Load the GDT for the BSP
    gdt_init();

    // Load the IDT and register the ISRs and IRQs for the BSP
    interrupts_init();
    // TODO: Activate all interrupts with STI once IRQs are implemented
    // interrupts_activate();

    // Test software interrupts
    vga_print(vga, "Triggering INT 0x02...");
    asm volatile("int $0x02");
    vga_println(vga, " [DONE]");

    vga_print(vga, "Triggering INT 0x03...");
    asm volatile("int $0x03");
    vga_println(vga, " [DONE]");

    vga_print(vga, "Triggering INT 0x04...");
    asm volatile("int $0x04");
    vga_println(vga, " [DONE]");

    vga_print(vga, "Triggering INT 0x1f...");
    asm volatile("int $0x1f");
    vga_println(vga, " [DONE]");
}
