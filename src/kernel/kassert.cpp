#include <drivers/vga.hpp>
#include <kernel/kassert.hpp>
#include <kernel/kpanic.hpp>
#include <kernel/platform/platform.hpp>

namespace kernel
{

void _assert_failure(
    const char *msg,
    const char *file,
    usize /*line*/,
    const char *function
)
{
    drivers::WRITER.put_string("*** ASSERTION FAILURE ***\n");
    drivers::WRITER.put_string("-> ");
    drivers::WRITER.put_string(msg);
    drivers::WRITER.new_line();
    drivers::WRITER.put_string("-> at ");
    drivers::WRITER.put_string(file);
    // TODO: Add support for outputting numbers
    // drivers::WRITER.put_string(":");
    // drivers::WRITER.put_string(line);
    drivers::WRITER.put_string(" in ");
    drivers::WRITER.put_string(function);
    drivers::WRITER.new_line();

    KPANIC("Cannot continue due to assertion failure");
}

} // namespace kernel
