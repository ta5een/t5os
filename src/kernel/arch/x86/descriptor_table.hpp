#pragma once

#include <kernel/kassert.hpp>
#include <lib/constants.hpp>
#include <lib/integers.hpp>

namespace kernel
{

/**
 * A data structure describing a pointer to a particular descriptor table.
 */
struct [[gnu::packed]] DescriptorTableRegister
{
  public:
    /**
     * The size of the descriptor table.
     */
    u16 limit{0};
    /**
     * The linear address of the descriptor table.
     */
    void *base{nullptr};
};

static_assert(sizeof(DescriptorTableRegister) == 6);

template <typename Entry, usize NEntries>
using DescriptorTable = Entry[NEntries];

} // namespace kernel
