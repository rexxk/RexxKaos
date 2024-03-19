#pragma once

#include "core/types.h"


enum class MemoryRegionType
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
    uint32_t RegionType;

};

