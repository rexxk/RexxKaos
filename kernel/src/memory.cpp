#include "memory/memory.h"
#include "console/console.h"

#include "container/vector.h"


extern uint64_t bootDataAddress;

extern "C" void _CopyMemory(void* destination, void* source, uint64_t length);
extern "C" void _SetMemory(void* destination, uint64_t value, uint64_t count);


static MemoryInformation* s_MemoryInformation = nullptr;

static FirmVector<MemoryRegion> s_MemoryRegions;

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
    s_MemoryRegions.Clear();
    
    s_MemoryInformation = (MemoryInformation*)bootDataAddress;
    
    for (uint16_t i = 0; i < s_MemoryInformation->EntryCount; i++)
    {
        MemoryRegion* region = (MemoryRegion*)&s_MemoryInformation->MemoryRegionPointer + ((i * s_MemoryInformation->EntrySize) / sizeof(MemoryRegion));

        s_MemoryRegions.PushBack(*region);
    }

    PrintString("Memory regions:\n");

    for (uint64_t i = 0; i < s_MemoryRegions.Size(); i++)
    {
        MemoryRegion& region = s_MemoryRegions.At(i);

        PrintString(" - %x : (%x bytes) [%s]\n", region.BaseAddress, region.Length, MemoryRegionTypeToString((MemoryRegionType)region.RegionType));
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
