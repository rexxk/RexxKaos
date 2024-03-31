#include "memory/PhysicalMemory.h"

#include "core/types.h"

#include "console/console.h"

// Physical memory map is located at 16 MB marker
static uint8_t* s_PhysicalMemoryMap = (uint8_t*)0x1000000;

static constexpr uint64_t s_PhysicalPageSize = 0x200000;



PhysicalMemory::PhysicalMemory(uint64_t totalPhysicalMemorySize)
{
    m_DirectoryEntries = totalPhysicalMemorySize / s_PhysicalPageSize;

    PrintString("Total physical memory map entries: %d\n", m_DirectoryEntries);
}
