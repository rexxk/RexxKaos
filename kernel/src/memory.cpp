#include "memory/memory.h"
#include "console/console.h"

#include "container/vector.h"


extern uint64_t bootDataAddress;

extern "C" void _CopyMemory(void* destination, void* source, uint64_t length);
extern "C" void _SetMemory(void* destination, uint64_t value, uint64_t count);


static BIOSMemoryInformation* s_MemoryRegionInformation = nullptr;
static MemoryInformation s_MemoryInformation;

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


const MemoryInformation& GetMemoryInformation()
{
    s_MemoryRegions.Clear();

    s_MemoryInformation.TotalMemorySize = 0x100000;
    s_MemoryInformation.MemoryPageSize = 0x200000;

    s_MemoryRegionInformation = (BIOSMemoryInformation*)bootDataAddress;
    
    for (uint16_t i = 0; i < s_MemoryRegionInformation->EntryCount; i++)
    {
        MemoryRegion* region = (MemoryRegion*)&s_MemoryRegionInformation->MemoryRegionPointer + ((i * s_MemoryRegionInformation->EntrySize) / sizeof(MemoryRegion));

        s_MemoryRegions.PushBack(*region);
    }

    PrintString("Available memory regions:\n");

    for (uint64_t i = 0; i < s_MemoryRegions.Size(); i++)
    {
        MemoryRegion& region = s_MemoryRegions.At(i);

        if (region.RegionType == MemoryRegionType::Usable)
        {
            PrintString(" - %x : (%x bytes) [%s]\n", region.BaseAddress, region.Length, MemoryRegionTypeToString((MemoryRegionType)region.RegionType));

            s_MemoryInformation.TotalMemorySize += region.Length;
        }
    }

    PrintString("Total amount of memory: %d kBytes", s_MemoryInformation.TotalMemorySize / 1024);

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
