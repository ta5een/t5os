#include <drivers/vga.hpp>
#include <kernel/kassert.hpp>
#include <kernel/kpanic.hpp>
#include <kernel/platform/platform.hpp>

namespace kernel
{

void _on_assert(
    const char *msg,
    const char *file,
    usize line,
    const char *function
)
{
#if ARCH(I386)
    // NOLINTNEXTLINE(hicpp-no-assembler)
    asm volatile("cli");
#else
#    error "`_on_assert` is not supported for this architecture"
#endif

    drivers::WRITER.put_string("*** ASSERTION FAILURE ***\n");
    drivers::WRITER.put_string("-> ");
    drivers::WRITER.put_string(msg);
    drivers::WRITER.new_line();
    drivers::WRITER.put_string("-> at ");
    drivers::WRITER.put_string(file);
    drivers::WRITER.put_string(":");
    drivers::WRITER.put_integer(drivers::VgaWriter::Unsigned, line);
    drivers::WRITER.new_line();
    drivers::WRITER.put_string("-> in ");
    drivers::WRITER.put_string(function);
    drivers::WRITER.new_line();

    KPANIC("Cannot continue due to assertion failure");
}

} // namespace kernel
