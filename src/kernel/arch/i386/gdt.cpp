#include <kernel/arch/i386/gdt.hpp>
#include <lib/constants.hpp>

namespace kernel
{

const u8 DB_BIT_MASK = 1 << 6;
const u8 G_BIT_MASK = 1 << 7;

GlobalDescriptorTable::GlobalDescriptorTable()
    : _null_segment_selector(0, 0, 0),
      _unused_segment_selector(0, 0, 0),
      _code_segment_selector(0, 64 * lib::MiB, 0x9A),
      _data_segment_selector(0, 64 * lib::MiB, 0x92)
{
    // Define the size (limit) and start (base) of the descriptor table
    u32 data[2];
    data[0] = sizeof(GlobalDescriptorTable) << 16;
    data[1] = (u32)this;
    asm volatile("lgdt (%0)"             // Load the descriptor table
                 :                       // No output operands
                 : "p"(((u8 *)data) + 2) // Pointer to array as input operand
    );
}

GlobalDescriptorTable::~GlobalDescriptorTable() {}

auto GlobalDescriptorTable::code_segment_selector() const -> u16
{
    return (u8 *)&_code_segment_selector - (u8 *)this;
}

auto GlobalDescriptorTable::data_segment_selector() const -> u16
{
    return (u8 *)&_data_segment_selector - (u8 *)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(
    u32 base,
    u32 limit,
    u8 access_byte
)
{
    u8 *target = (u8 *)this;

    if (limit <= (1 << 16))
    {
        // 16-bit address space
        target[6] = DB_BIT_MASK;
    }
    else
    {
        // 32-bit address space
        //
        // Now we have to squeeze the 32-bit limit entry into 2.5 registers
        // (20 bits). This is done by discarding the 12 least significant
        // bits, but this is only legal if they are all equal to 1. If the
        // last bits aren't all 1, we have to set them to 1, but this would
        // increase the limit, which may make the limit go beyong the
        // physical limit or overlap with other segments. Thus, we have to
        // compensate this by decreasing a higher bit (and might have up to
        // (2**12)-1 wasted bytes behind the used memory).
        //
        // This is standard practice, as explained in a StackOverflow
        // comment [1].
        //
        // [1] https://stackoverflow.com/a/55970477/10052039
        if ((limit & 0xFFF) != 0xFFF)
        {
            limit = (limit >> 12) - 1;
        }
        else
        {
            limit = limit >> 12;
        }

        // Set the Granularity and the Default Operand Size flags
        target[6] = G_BIT_MASK | DB_BIT_MASK;
    }

    // Encode the limit
    target[0] = limit & 0xFF;         // First two bytes
    target[1] = (limit >> 8) & 0xFF;  // Next two bytes
    target[6] |= (limit >> 16) & 0xF; // Last nibble (half-byte)

    // Encode the base
    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;

    // Encode the access byte
    // NOTE: This variable is named `flags` in the tutorial
    target[5] = access_byte;
}

auto GlobalDescriptorTable::SegmentDescriptor::base() const -> u32
{
    u8 *target = (u8 *)this;
    u32 result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

auto GlobalDescriptorTable::SegmentDescriptor::limit() const -> u32
{
    u8 *target = (u8 *)this;
    u32 result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    // If the limit entry is a 32-bit entry (and thus the last 12 bits were
    // discarded), we "unshift" the entry and set all the last 12 bits to 1.
    if ((target[6] & G_BIT_MASK) == G_BIT_MASK)
    {
        result = (result << 12) | 0xFFF;
    }

    return result;
}

} // namespace kernel
