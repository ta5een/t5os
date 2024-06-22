#include <kernel/platform/x86/gdt.hpp>
#include <lib/constants.hpp>

namespace kernel
{

const u8 DB_BIT_MASK = 1U << 6U;
const u8 G_BIT_MASK = 1U << 7U;

const u32 CODE_SEGMENT_LIMIT = 64 * lib::MiB;
const u32 DATA_SEGMENT_LIMIT = 64 * lib::MiB;

struct [[gnu::packed]] GlobalDescriptorTableRegister
{
  public:
    u16 limit;
    void *base;
};

GlobalDescriptorTable::GlobalDescriptorTable()
    : m_null_segment_selector(SegmentDescriptor::Empty)
    , m_unused_segment_selector(SegmentDescriptor::Empty)
    , m_code_segment_selector(
          SegmentDescriptor::WithOptions,
          {.base = 0, .limit = CODE_SEGMENT_LIMIT, .access_byte = 0x9A}
      )
    , m_data_segment_selector(
          SegmentDescriptor::WithOptions,
          {.base = 0, .limit = DATA_SEGMENT_LIMIT, .access_byte = 0x92}
      )
{
}

void GlobalDescriptorTable::load()
{
    // Define the size (limit) and start (base) of the descriptor table
    GlobalDescriptorTableRegister data{
        .limit = sizeof(GlobalDescriptorTable),
        .base = (void *)this,
    };
    // NOLINTNEXTLINE(hicpp-no-assembler)
    asm volatile("lgdt (%0)" : : "p"(&data));
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(
    GlobalDescriptorTable::SegmentDescriptor::EmptyTag /*unused*/
)
{
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(
    GlobalDescriptorTable::SegmentDescriptor::WithOptionsTag /*unused*/,
    Options options
)
    : SegmentDescriptor(Empty)
{
    if (options.limit <= (1 << 16))
    {
        // 16-bit address space
        m_flags_limit_16_19 = DB_BIT_MASK;
    }
    else
    {
        // 32-bit address space
        //
        // Now, we have to squeeze the 32-bit limit entry into 2.5 registers (20
        // bits). This is done by discarding the 12 least significant bits. This
        // won't be a problem if the last bits are all equal to 1. Otherwise, we
        // will look the other way and believe they are in fact equal to 1.
        //
        // Of course, this may increase the limit to go beyond the physical
        // limit or overlap with other segments. To compensate for this, we will
        // decrease the provided limit by a higher bit (and might have up to
        // (2**12)-1 wasted bytes behind the used memory).
        //
        // This is standard practice, as explained in the following
        // StackOverflow comment:
        //
        // https://stackoverflow.com/a/55970477/10052039
        if ((options.limit & 0xFFF) != 0xFFF)
        {
            options.limit = (options.limit >> 12) - 1;
        }
        else
        {
            options.limit = options.limit >> 12;
        }

        // Set the Granularity and the Default Operand Size flags
        m_flags_limit_16_19 = G_BIT_MASK | DB_BIT_MASK;
    }

    // Encode the limit
    m_limit_0_15 = options.limit & 0xFFFF;              // First two bytes
    m_flags_limit_16_19 |= (options.limit >> 16) & 0xF; // Last half-byte

    // Encode the base
    m_base_0_15 = options.base & 0xFFFF;        // First two bytes
    m_base_16_23 = (options.base >> 16) & 0xFF; // Next byte
    m_base_24_31 = (options.base >> 24) & 0xFF; // Last byte

    // Encode the access byte
    // NOTE: This variable is named `flags` in the tutorial
    m_access_byte = options.access_byte;
}

u32 GlobalDescriptorTable::SegmentDescriptor::base() const
{
    u32 result = m_base_24_31;
    result = (result << 8) + m_base_16_23;
    result = (result << 16) + m_base_0_15;
    return result;
}

u32 GlobalDescriptorTable::SegmentDescriptor::limit() const
{
    u32 result = m_flags_limit_16_19 & 0xF;
    result = (result << 16) + m_limit_0_15;

    // If the limit entry is a 32-bit entry (and thus the last 12 bits were
    // discarded), we "unshift" the entry and set all the last 12 bits to 1.
    if ((m_flags_limit_16_19 & G_BIT_MASK) == G_BIT_MASK)
    {
        result = (result << 12) | 0xFFF;
    }

    return result;
}

} // namespace kernel
