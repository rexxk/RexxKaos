#include "disk.h"

#include <map>
#include <cstring>

#include <iostream>

#include <fstream>

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

std::shared_ptr<DiskMedia> DiskMedia::Create(const MediaDescriptor& mediaDescriptor)
{
    return std::make_shared<DiskMedia>(mediaDescriptor);
}

std::shared_ptr<DiskMedia> DiskMedia::Create(DiskMediaType mediaType)
{
    return std::make_shared<DiskMedia>(mediaType);
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
}

std::vector<uint8_t>& DiskMedia::ReadSector(uint32_t sector, uint32_t cylinder, uint32_t head)
{
    uint32_t diskSector = (cylinder * m_Descriptor.Heads * m_Descriptor.Sectors) + (head * m_Descriptor.Sectors) + (sector - 1);

    std::cout << "disk sector: " << diskSector << "\n";
    return s_SectorData[diskSector];
}

void DiskMedia::SaveImage(const std::string& filename)
{
    std::ofstream fs(filename, std::ios::out | std::ios::binary);

    if (!fs.is_open())
    {
        std::cout << "Failed to create image file " << filename << "\n";
        return;
    }

    for (auto& sector : s_SectorData)
    {
        auto& sectorData = sector.second;

        fs.write((const char*)sectorData.data(), sectorData.size());
    }

    std::cout << "Sectors written to image file: " << filename << "\n";

    fs.close();
}

void DiskMedia::WriteToSector(uint32_t sector, const char* data, uint32_t length, uint32_t offset)
{
    std::vector<uint8_t>& sectorData = s_SectorData[sector];

    std::memcpy(sectorData.data() + offset, data, length);
}
