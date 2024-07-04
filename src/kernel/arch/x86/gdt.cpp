#include <kernel/arch/x86/descriptor_table.hpp>
#include <kernel/arch/x86/gdt.hpp>
#include <kernel/kassert.hpp>
#include <lib/constants.hpp>
#include <lib/semantics.hpp>

namespace kernel
{

void GlobalDescriptorTable::write_entry(
    usize idx,
    const SegmentDescriptor &&segment
)
{
    KASSERT(idx < NUM_ENTRIES && "index out of range");
    m_entries[idx] = lib::move(segment);
}

void GlobalDescriptorTable::load() const
{
    // Define the size (limit) and start (base) of the descriptor table
    static DescriptorTableRegister gdtr;
    gdtr.limit = sizeof(GlobalDescriptorTable);
    gdtr.base = (void *)this;

    // From:
    // https://github.com/SerenityOS/serenity/blob/9f0ab281ced15e30f511c91bc1b06deb8f79269a/Kernel/Arch/x86_64/Processor.cpp#L752-L753
    // https://git.bananymous.com/Bananymous/banan-os/src/commit/f18c33563d52b4286707794d045ad9d52e758380/kernel/include/kernel/GDT.h#L137-L140
    // NOLINTNEXTLINE(hicpp-no-assembler)
    asm volatile("lgdt %0" : : "m"(gdtr) : "memory");
}

} // namespace kernel
