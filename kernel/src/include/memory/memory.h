#pragma once

#include "core/types.h"
#include "MemoryRegion.h"



struct __attribute__((packed)) MemoryInformation
{
    uint16_t LowMemory;
    uint16_t EntryCount;
    uint16_t EntrySize;

    MemoryRegion* MemoryRegionPointer;
};



MemoryInformation* GetMemoryInformation();


void CopyMemory(void* destination, void* source, uint64_t length);
void SetMemory(void* destination, uint64_t value, uint64_t count);
