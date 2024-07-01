#include <drivers/vga.hpp>
#include <kernel/arch/platform.hpp>
#include <kernel/kassert.hpp>
#include <kernel/kpanic.hpp>

namespace kernel
{

void _on_assert(
    const char *msg,
    const char *file,
    usize line,
    const char *function
)
{
#if ARCH(X86_32)
    // NOLINTNEXTLINE(hicpp-no-assembler)
    asm volatile("cli");
#else
#    error "`_on_assert` is not supported for this architecture"
#endif

    const auto &vga = drivers::VgaWriter::instance();
    vga.put_string("*** ASSERTION FAILURE ***\n");
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

    KPANIC("Cannot continue due to assertion failure");
}

} // namespace kernel
