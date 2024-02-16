#include "fat12.h"

#include <iostream>
#include <cstring>

#include <vector>

#ifdef _WIN32
#pragma pack(push, 1)
struct BootParameterBlock
#endif

#ifdef __linux__
struct __attribute__((packed)) BootParameterBlock
#endif
{
    uint8_t OEMName[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectorsCount = 1;
    uint8_t NumberOfFATs;
    uint16_t MaxRootDirectoryEntries;
    uint16_t TotalSectors;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t NumberOfHeads;
    uint32_t HiddenSectors;
    uint32_t TotalSectorsLarge;

    uint8_t DriveNumber;
    uint8_t FlagsWindowsNT;
    uint8_t BootSignature;
    uint32_t VolumeID;
    uint8_t VolumeLabel[11];
    uint8_t FileSystemType[8];
};

#ifdef _WIN32
#pragma pack(pop, 1)
#endif

static BootParameterBlock s_BPBData;


struct FAT12Data
{
    std::vector<uint16_t> FATLocations;
    uint16_t RootDirectoryLocation;
    uint32_t TotalSectorCount;
};

static FAT12Data s_FAT12Data;

#ifdef _WIN32
#pragma pack(push, 1)
struct FAT12DirectoryEntry
#endif

#ifdef __linux__
struct __attribute__((packed)) FAT12DirectoryEntry
#endif
{
    uint8_t Filename[11];
    uint8_t Attributes;
    uint8_t ReservedNT;
    uint8_t CreationTimeHundreds;
    uint16_t CreationTime;
    uint16_t CreationDate;
    uint16_t LastAccessedDate;
    uint16_t FirstClusterHigh = 0;
    uint16_t LastModificationTime;
    uint16_t LastModificationDate;
    uint16_t FirstCluster;
    uint32_t FileSize;
};

#ifdef _WIN32
#pragma pack(pop, 1)
#endif

Fat12::Fat12(std::shared_ptr<DiskMedia> diskMedia)
    : m_DiskMedia(diskMedia)
{
    CreateFilesystem();
}


void Fat12::CreateFilesystem()
{
    std::cout << "FAT12 filesystem initialization\n";

    DiskMedia::MediaDescriptor mediaDescriptor = m_DiskMedia->GetMediaDescriptor();

    uint32_t totalSectorCount = m_DiskMedia->GetTotalSectorCount();

    if (totalSectorCount > 0xFFFF)
    {
        s_BPBData.TotalSectors = 0;
        s_BPBData.TotalSectorsLarge = totalSectorCount;
    }
    else
    {
        s_BPBData.TotalSectors = totalSectorCount;
        s_BPBData.TotalSectorsLarge = 0;
    }

    std::memcpy(s_BPBData.OEMName, "REXXKAOS", 8);
    s_BPBData.BytesPerSector = mediaDescriptor.SectorSize;
    s_BPBData.SectorsPerCluster = 1;
    s_BPBData.ReservedSectorsCount = 1;
    s_BPBData.NumberOfFATs = 2;
    s_BPBData.MaxRootDirectoryEntries = 224;
    s_BPBData.MediaDescriptor = 0xF0;
    s_BPBData.SectorsPerFAT = 9;
    s_BPBData.SectorsPerTrack = mediaDescriptor.Sectors;
    s_BPBData.NumberOfHeads = mediaDescriptor.Heads;
    s_BPBData.HiddenSectors = 0;

    s_BPBData.DriveNumber = 0;
    s_BPBData.FlagsWindowsNT = 0;
    s_BPBData.BootSignature = 0x29;
    s_BPBData.VolumeID = 0xDEADBEEF;
    std::memcpy(s_BPBData.VolumeLabel, "REXXKAOS   ", 11);
    std::memcpy(s_BPBData.FileSystemType, "FAT12   ", 8);

    std::cout << "Size of boot parameter block: " << sizeof(s_BPBData) << " bytes\n";

    m_DiskMedia->WriteToSector(0, (const char*)&s_BPBData, sizeof(s_BPBData), 3);

    CalculateFATData();
}

void Fat12::CalculateFATData()
{
    uint16_t location = 1 + s_BPBData.HiddenSectors;

    for (uint32_t i = 0; i < s_BPBData.NumberOfFATs; i++)
    {
        s_FAT12Data.FATLocations.push_back(location);
        std::cout << "FAT location: " << location << "\n";

        location += s_BPBData.SectorsPerFAT;
    }

    s_FAT12Data.RootDirectoryLocation = location;

    std::cout << "Root directory: " << location << "\n";

    if (s_BPBData.TotalSectors == 0)
        s_FAT12Data.TotalSectorCount = s_BPBData.TotalSectorsLarge;
    else
        s_FAT12Data.TotalSectorCount = s_BPBData.TotalSectors;

    std::cout << "Total sector count: " << s_FAT12Data.TotalSectorCount << "\n";

    std::cout << "Directory entry size: " << sizeof(FAT12DirectoryEntry) << "\n";
}
