#include "disk.h"

#include <map>
#include <cstring>

#include <iostream>


static std::map<uint32_t, std::vector<uint8_t>> s_SectorData;


DiskMedia::MediaDescriptor CreateMediaDescriptorFromType(DiskMediaType mediaType)
{
    switch (mediaType)
    {
        case DiskMediaType::DD_720: return DiskMedia::MediaDescriptor { 2, 80, 9, 512 };
        case DiskMediaType::DD_880: return DiskMedia::MediaDescriptor { 2, 80, 11, 512 };
        case DiskMediaType::HD_1440: return DiskMedia::MediaDescriptor { 2, 80, 18, 512 };
    }

    return DiskMedia::MediaDescriptor {0};
}


DiskMedia::DiskMedia(const MediaDescriptor& mediaDescriptor)
    : m_Descriptor(mediaDescriptor)
{
    SetupSectors();
}

DiskMedia::DiskMedia(DiskMediaType mediaType)
{
    m_Descriptor = CreateMediaDescriptorFromType(mediaType);

    SetupSectors();
}

void DiskMedia::SetupSectors()
{
    m_SectorCount = m_Descriptor.Cylinders * m_Descriptor.Heads * m_Descriptor.Sectors;

    for (uint32_t i = 0; i < m_SectorCount; i++)
    {
        s_SectorData[i].resize(m_Descriptor.SectorSize);
        std::memset(s_SectorData[i].data(), 0, m_Descriptor.SectorSize);
    }
}

std::vector<uint8_t>& DiskMedia::ReadSector(uint32_t diskSector)
{

    uint32_t sector = (diskSector % m_Descriptor.Sectors) + 1;
    uint32_t cylinder = (diskSector / m_Descriptor.Sectors) / m_Descriptor.Heads;
    uint32_t head = (diskSector / m_Descriptor.Sectors) % m_Descriptor.Heads; 

    std::cout << "sector: " << sector << ", cylinder: " << cylinder << ", head: " << head << "\n";

    return s_SectorData[diskSector];
//Sector = (LBA mod SectorsPerTrack)+1
//Cylinder = (LBA/SectorsPerTrack)/NumHeads
//Head = (LBA/SectorsPerTrack) mod NumHeads
}

std::vector<uint8_t>& DiskMedia::ReadSector(uint32_t sector, uint32_t cylinder, uint32_t head)
{
//    (C x TH x TS) + (H x TS) + (S - 1) = LBA
    uint32_t diskSector = (cylinder * m_Descriptor.Heads * m_Descriptor.Sectors) +
                            (head * m_Descriptor.Sectors) + (sector - 1);

    std::cout << "disk sector: " << diskSector << "\n";
    return s_SectorData[diskSector];
}
