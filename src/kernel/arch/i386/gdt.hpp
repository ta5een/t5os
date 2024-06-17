#pragma once

#include <lib/integers.hpp>

namespace kernel
{
    struct GlobalDescriptorTable
    {
    public:
        /**
         * An entry in a Global Descriptor Table.
         */
        struct [[gnu::packed]] SegmentDescriptor
        {
        public:
            SegmentDescriptor(u32 base, u32 limit, u8 access_byte);
            auto base() const -> u32;
            auto limit() const -> u32;

        private:
            u16 _limit_0_15;
            u16 _base_0_15;
            u8 _base_16_23;
            u8 _access_byte;
            u8 _flags_limit_16_19;
            u8 _base_24_31;
        };

        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        auto code_segment_selector() const -> u16;
        auto data_segment_selector() const -> u16;

    private:
        const SegmentDescriptor _null_segment_selector;
        const SegmentDescriptor _unused_segment_selector;
        const SegmentDescriptor _code_segment_selector;
        const SegmentDescriptor _data_segment_selector;
    };
} // namespace kernel
