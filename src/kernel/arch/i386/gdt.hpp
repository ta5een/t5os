#pragma once

#include <lib/integers.hpp>

namespace kernel
{

class GlobalDescriptorTable
{
  public:
    /**
     * An entry in a Global Descriptor Table.
     */
    class [[gnu::packed]] SegmentDescriptor
    {
      public:
        SegmentDescriptor(u32 base, u32 limit, u8 access_byte);

        /**
         * The decoded linear address where this segment begins.
         */
        auto base() const -> u32;

        /**
         * The decoded limit of this segment, respecting the page granularity
         * such that it value ranges from 1 byte to 4 GiB.
         */
        auto limit() const -> u32;

      private:
        u16 _limit_0_15;
        u16 _base_0_15;
        u8 _base_16_23;
        u8 _access_byte;
        u8 _flags_limit_16_19;
        u8 _base_24_31;
    };

    /**
     * Construct a GlobalDescriptorTable with predefined segments.
     */
    GlobalDescriptorTable();

    /**
     * TODO: No-op.
     */
    ~GlobalDescriptorTable();

    /**
     * The raw value of the kernel code segment selector.
     */
    auto code_segment_selector() const -> u16
    {
        return (u8 *)&_code_segment_selector - (u8 *)this;
    }

    /**
     * The raw value of the kernel data segment selector.
     */
    auto data_segment_selector() const -> u16
    {
        return (u8 *)&_data_segment_selector - (u8 *)this;
    }

  private:
    const SegmentDescriptor _null_segment_selector;
    const SegmentDescriptor _unused_segment_selector;
    const SegmentDescriptor _code_segment_selector;
    const SegmentDescriptor _data_segment_selector;
};

} // namespace kernel
