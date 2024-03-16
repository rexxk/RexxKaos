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



struct __attribute__((packed)) _RawMemoryRegion
{
    uint64_t BaseAddress;
    uint64_t Length;
    uint32_t RegionType;

};

struct __attribute__((packed)) MemoryInformation
{
    uint16_t LowMemory;
    uint16_t EntryCount;
    uint16_t EntrySize;

    _RawMemoryRegion* MemoryRegions;
};


struct MemoryRegion
{
    uint64_t BaseAddress;
    uint64_t Length;
    uint32_t RegionType;

    MemoryRegion* Next = nullptr;
};


MemoryInformation* GetMemoryInformation();


void CopyMemory(void* destination, void* source, uint64_t length);
void SetMemory(void* destination, uint64_t value, uint64_t count);
