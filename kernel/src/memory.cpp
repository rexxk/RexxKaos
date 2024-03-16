#include "include/memory.h"
#include "include/console.h"


extern uint64_t bootDataAddress;

extern "C" void _CopyMemory(void* destination, void* source, uint64_t length);
extern "C" void _SetMemory(void* destination, uint64_t value, uint64_t count);


static MemoryInformation* s_MemoryInformation = nullptr;



MemoryInformation* GetMemoryInformation()
{
    s_MemoryInformation = (MemoryInformation*)bootDataAddress;

    PrintString("Memory information pointer: %x\n", bootDataAddress);

    PrintString(" - entry count: %d\n", s_MemoryInformation->EntryCount);
    PrintString(" - entry size: %d\n", s_MemoryInformation->EntrySize);
    PrintString(" - region start: %x\n", &s_MemoryInformation->MemoryRegions);

    for (uint16_t i = 0; i < s_MemoryInformation->EntryCount; i++)
    {
        _RawMemoryRegion* region = (_RawMemoryRegion*)&s_MemoryInformation->MemoryRegions + ((i * s_MemoryInformation->EntrySize) / sizeof(_RawMemoryRegion));


        PrintString("  + %x : (%x bytes) [%d]\n", region->BaseAddress, region->Length, region->RegionType);

    }

    return s_MemoryInformation;
}



void CopyMemory(void* destination, void* source, uint64_t length)
{
    _CopyMemory(destination, source, length);
}

void SetMemory(void* destination, uint64_t value, uint64_t count)
{
    _SetMemory(destination, value, count);
}
