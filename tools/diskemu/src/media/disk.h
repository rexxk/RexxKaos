#pragma once

#include <cstdint>
#include <vector>



enum class DiskMediaType
{
    DD_720,
    DD_880,
    HD_1440,
};


class DiskMedia
{
public:
    struct MediaDescriptor
    {
        uint8_t Heads;
        uint8_t Cylinders;
        uint32_t Sectors;

        uint16_t SectorSize = 512;
    };

public:
    DiskMedia(const MediaDescriptor& mediaDescriptor);
    DiskMedia(DiskMediaType mediaType);

    const MediaDescriptor& GetMediaDescriptor() const { return m_Descriptor; }

    std::vector<uint8_t>& ReadSector(uint32_t diskSector);
    std::vector<uint8_t>& ReadSector(uint32_t sector, uint32_t cylinder, uint32_t head);

private:
    void SetupSectors();

private:
    MediaDescriptor m_Descriptor;

    uint32_t m_SectorCount = 0;
};
