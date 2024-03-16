#pragma once

#include "include/types.h"


enum class MemoryRegionType
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
    uint32_t RegionType;

};
