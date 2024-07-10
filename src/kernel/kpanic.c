#include <kernel/arch/x86/devices/vga.h>
#include <kernel/kpanic.h>

void
kernel_on_panic(const char *msg, const char *location, const char *function)
{
    // TODO: Write to VGA and serial port...
    struct vga *vga = vga_get();
    vga_println(vga, "*** KERNEL PANIC ***");
    vga_print(vga, "-> ");
    vga_println(vga, msg);
    vga_print(vga, "-> at ");
    vga_println(vga, location);
    vga_print(vga, "-> in ");
    vga_println(vga, function);

    // TODO: Move this to generic CPU interface
#if defined(__i386__) || defined(__x86_64__)
    while (true)
    {
        asm volatile("cli; hlt");
    }
#else
    #error "`kernel_on_panic` not implemented for current architecture"
#endif
}
