#pragma once

#include <cstdint>
#include <vector>

#include <string>

#include <memory>


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
    static std::shared_ptr<DiskMedia> Create(const MediaDescriptor& mediaDescriptor);
    static std::shared_ptr<DiskMedia> Create(DiskMediaType mediaType);

    DiskMedia(const MediaDescriptor& mediaDescriptor);
    DiskMedia(DiskMediaType mediaType);

    const MediaDescriptor& GetMediaDescriptor() const { return m_Descriptor; }

    const uint32_t GetTotalSectorCount() const { return m_SectorCount; }

    std::vector<uint8_t>& ReadSector(uint32_t diskSector);
    std::vector<uint8_t>& ReadSector(uint32_t sector, uint32_t cylinder, uint32_t head);

    void WriteToSector(uint32_t sector, const char* data, uint32_t length, uint32_t offset = 0);

    void SaveImage(const std::string& filename);

private:
    void SetupSectors();

private:
    MediaDescriptor m_Descriptor;

    uint32_t m_SectorCount = 0;
};
