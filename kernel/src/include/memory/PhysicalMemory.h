#pragma once

#include "core/types.h"


enum class PhysicalMemoryState : uint8_t
{
    Free,
    System,
    User,
};



class PhysicalMemory
{
public:
    PhysicalMemory(uint64_t totalPhysicalMemorySize);    

private:
    uint64_t m_DirectoryEntries = 0;

};
