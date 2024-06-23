#include <drivers/vga.hpp>
#include <kernel/kpanic.hpp>
#include <kernel/platform/platform.hpp>

namespace kernel
{

void _on_panic(
    const char *msg,
    const char *file,
    usize line,
    const char *function
)
{
    drivers::WRITER.put_string("*** KERNEL PANIC ***\n");
    drivers::WRITER.put_string("-> ");
    drivers::WRITER.put_string(msg);
    drivers::WRITER.new_line();
    drivers::WRITER.put_string("-> at ");
    drivers::WRITER.put_string(file);
    drivers::WRITER.put_string(":");
    // TODO: Ideally, we shouldn't cast from usize to ssize
    drivers::WRITER.put_integer((ssize)line);
    drivers::WRITER.new_line();
    drivers::WRITER.put_string("-> in ");
    drivers::WRITER.put_string(function);
    drivers::WRITER.new_line();

// TODO: Call arch-specific 'halt' instruction
#if ARCH(I386)
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
