#pragma once

#include "core/types.h"


enum class MemoryRegionType : uint32_t
{
    Usable = 1,
    Reserved,
    ACPIReclaimable,
    ACPINVS,
    Bad,
};


struct MemoryRegion
{
    uint64_t BaseAddress;
    uint64_t Length;
    MemoryRegionType RegionType;

} __attribute__((packed));

