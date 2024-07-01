#include <drivers/vga.hpp>
#include <kernel/arch/platform.hpp>
#include <kernel/kpanic.hpp>

namespace kernel
{

void _on_panic(
    const char *msg,
    const char *file,
    usize line,
    const char *function
)
{
    const auto &vga = drivers::VgaWriter::instance();
    vga.put_string("*** KERNEL PANIC ***\n");
    vga.put_string("-> ");
    vga.put_string(msg);
    vga.new_line();
    vga.put_string("-> at ");
    vga.put_string(file);
    vga.put_string(":");
    vga.put_integer(drivers::VgaWriter::Unsigned, line);
    vga.new_line();
    vga.put_string("-> in ");
    vga.put_string(function);
    vga.new_line();

// TODO: Call arch-specific 'halt' instruction
#if ARCH(X86_32)
    while (true)
    {
        // NOLINTNEXTLINE(hicpp-no-assembler)
        asm volatile("cli; hlt");
    }
#else
#    error "`_on_panic` is not supported for this architecture"
#endif
}

} // namespace kernel
