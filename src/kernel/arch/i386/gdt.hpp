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
            u16 limit_0_15_;
            u16 base_0_15_;
            u8 base_16_23_;
            u8 access_byte_;
            u8 flags_limit_16_19_;
            u8 base_24_31_;
        };

        [[nodiscard("Contains side effects")]]
        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        auto code_segment_selector() const -> u16;
        auto data_segment_selector() const -> u16;

    private:
        const SegmentDescriptor null_segment_selector_;
        const SegmentDescriptor unused_segment_selector_;
        const SegmentDescriptor code_segment_selector_;
        const SegmentDescriptor data_segment_selector_;
    };
} // namespace kernel
