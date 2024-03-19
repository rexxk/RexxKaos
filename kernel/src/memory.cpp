#include "memory/memory.h"
#include "console/console.h"


extern uint64_t bootDataAddress;

extern "C" void _CopyMemory(void* destination, void* source, uint64_t length);
extern "C" void _SetMemory(void* destination, uint64_t value, uint64_t count);


static MemoryInformation* s_MemoryInformation = nullptr;


const char* MemoryRegionTypeToString(MemoryRegionType type)
{
    switch (type)
    {
        case MemoryRegionType::Usable: return "Usable";
        case MemoryRegionType::Reserved: return "Reserved";
        case MemoryRegionType::ACPIReclaimable: return "ACPI Reclaimable";
        case MemoryRegionType::ACPINVS: return "ACPI NVS";
        case MemoryRegionType::Bad: return "Bad";
    }

    return "Unknown region type";
}


MemoryInformation* GetMemoryInformation()
{
    s_MemoryInformation = (MemoryInformation*)bootDataAddress;

    PrintString("Memory information pointer: %x\n", bootDataAddress);

    PrintString(" - entry count: %d\n", s_MemoryInformation->EntryCount);
    PrintString(" - entry size: %d\n", s_MemoryInformation->EntrySize);
    PrintString(" - region start: %x\n", &s_MemoryInformation->MemoryRegionPointer);

    for (uint16_t i = 0; i < s_MemoryInformation->EntryCount; i++)
    {
        MemoryRegion* region = (MemoryRegion*)&s_MemoryInformation->MemoryRegionPointer + ((i * s_MemoryInformation->EntrySize) / sizeof(MemoryRegion));

        PrintString("  + %x : (%x bytes) [%s]\n", region->BaseAddress, region->Length, MemoryRegionTypeToString((MemoryRegionType)region->RegionType));
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
