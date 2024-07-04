#pragma once

#include <kernel/arch/x86/descriptor_table.hpp>
#include <lib/integers.hpp>

namespace kernel
{

struct [[gnu::packed]] SegmentDescriptor
{
  public:
    static constexpr u8 ACCESS_ACCESSED = 1U;
    static constexpr u8 ACCESS_WRITEABLE = 1U << 1U;
    static constexpr u8 ACCESS_CONFORMING = 1U << 2U;
    static constexpr u8 ACCESS_EXECUTABLE = 1U << 3U;
    static constexpr u8 ACCESS_USER_SEGMENT = 1U << 4U;
    static constexpr u8 ACCESS_RING0 = 0U << 5U;
    static constexpr u8 ACCESS_RING1 = 1U << 5U;
    static constexpr u8 ACCESS_RING2 = 2U << 5U;
    static constexpr u8 ACCESS_RING3 = 3U << 5U;
    static constexpr u8 ACCESS_PRESENT = 1U << 7U;

    static constexpr u8 FLAG_LONGMODE = 1U << 1U;
    static constexpr u8 FLAG_DEFAULT_SIZE = 1U << 2U;
    static constexpr u8 FLAG_GRANULARITY = 1U << 3U;

    SegmentDescriptor() = default;

    SegmentDescriptor(u32 base, u32 limit, u8 access, u8 flags)
        : limit_0_15(limit & 0xffffU)
        , base_0_15(base & 0xffffU)
        , base_16_23((base >> 16U) & 0xffU)
        , access_byte(access)
        , flags_limit_16_19((flags << 4U) | ((limit >> 16U) & 0xfU))
        , base_24_31((base >> 24U) & 0xffU)
    {
    }

    u32 base() const
    {
        u32 result = base_0_15;
        result |= base_16_23 << 16U;
        result |= base_24_31 << 24U;
        return result;
    }

    u32 limit() const
    {
        u32 result = limit_0_15;
        result |= (flags_limit_16_19 & 0xfU) << 16U;
        return result;
    }

    u16 limit_0_15{0};
    u16 base_0_15{0};
    u8 base_16_23{0};
    u8 access_byte{0};
    u8 flags_limit_16_19{0};
    u8 base_24_31{0};
};

static_assert(sizeof(SegmentDescriptor) == 8);

class [[gnu::packed]] GlobalDescriptorTable
{
  public:
    static constexpr usize NUM_ENTRIES = 4U;
    static constexpr usize NULL_IDX = 0U;
    static constexpr usize UNUSED_IDX = 1U;
    static constexpr usize KERNEL_CS_IDX = 2U;
    static constexpr usize KERNEL_DS_IDX = 3U;

    using Entries = DescriptorTable<SegmentDescriptor, NUM_ENTRIES>;

    explicit GlobalDescriptorTable() = default;

    void write_entry(usize idx, const SegmentDescriptor &&segment);
    void load() const;

  private:
    Entries m_entries;
};

static_assert(
    sizeof(GlobalDescriptorTable) ==
    (sizeof(SegmentDescriptor) * GlobalDescriptorTable::NUM_ENTRIES)
);

} // namespace kernel
