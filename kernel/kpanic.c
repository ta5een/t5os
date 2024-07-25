#include <kernel/arch/cpu.h>
#include <kernel/arch/x86/devices/vga.h>
#include <kernel/kpanic.h>

void
kernel_on_panic(const char *msg, const char *location, const char *function)
{
    // TODO: Write to both VGA and serial port
    // TODO: VGA API should be abstracted into generic arch interface
    struct vga *vga = vga_get();
    vga_println(vga, "*** KERNEL PANIC ***");
    vga_print(vga, "-> ");
    vga_println(vga, msg);
    vga_print(vga, "-> at ");
    vga_println(vga, location);
    vga_print(vga, "-> in ");
    vga_println(vga, function);

    arch_cpu_halt();
}