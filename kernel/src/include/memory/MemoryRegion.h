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


struct __attribute__((packed)) MemoryRegion
{
    uint64_t BaseAddress;
    uint64_t Length;
    MemoryRegionType RegionType;

};

