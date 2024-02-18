#include "fat12.h"

#include <iostream>
#include <cstring>

#include <vector>



enum class FAT12DirectoryFlag
{
    ReadOnly = 0x1,
    Hidden = 0x2,
    System = 0x4,
    VolumeLabel = 0x8,
    Subdirectory = 0x10,
    Archive = 0x20,
    Device = 0x40,
    Unused = 0x80,
};

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

struct FAT12Data
{
    std::vector<uint16_t> FATLocations;
    std::vector<uint16_t> FATTable;
    std::vector<FAT12DirectoryEntry> DirectoryEntries;
    uint32_t FATEntries;
    uint16_t RootDirectoryLocation;
    uint32_t DataRegionStart;
    uint32_t TotalSectorCount;
};

static FAT12Data s_FAT12Data;


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

    StoreToImage();
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
    s_FAT12Data.DataRegionStart = s_FAT12Data.RootDirectoryLocation + (s_BPBData.MaxRootDirectoryEntries * sizeof(FAT12DirectoryEntry)) / s_BPBData.BytesPerSector;

    std::cout << "Root directory: " << location << "\n";
    std::cout << "Data region start: " << s_FAT12Data.DataRegionStart << "\n";

    if (s_BPBData.TotalSectors == 0)
        s_FAT12Data.TotalSectorCount = s_BPBData.TotalSectorsLarge;
    else
        s_FAT12Data.TotalSectorCount = s_BPBData.TotalSectors;

    std::cout << "Total sector count: " << s_FAT12Data.TotalSectorCount << "\n";

    s_FAT12Data.FATEntries = s_BPBData.SectorsPerFAT * s_BPBData.BytesPerSector * 1.5;

    std::cout << "FAT entries count: " << s_FAT12Data.FATEntries << "\n";

    s_FAT12Data.FATTable.resize(s_FAT12Data.FATEntries);

    SetFATEntry(0, (0xF << 8) + s_BPBData.MediaDescriptor);
    SetFATEntry(1, 0xFFF);

    std::cout << "FAT entry 0: " << std::hex << GetFATEntry(0) << std::dec << "\n";
    std::cout << "FAT entry 1: " << std::hex << GetFATEntry(1) << std::dec << "\n";

    s_FAT12Data.DirectoryEntries.resize(s_BPBData.MaxRootDirectoryEntries);
    std::cout << "Directory size: " << s_FAT12Data.DirectoryEntries.size() * sizeof(FAT12DirectoryEntry) << "\n";
    ClearDirectory(s_FAT12Data.RootDirectoryLocation);

    std::cout << "First free entry: " << FindFirstFreeFATEntry() << "\n";
}

void Fat12::ClearDirectory(uint32_t sector)
{
    for (uint32_t i = 0; i < s_BPBData.MaxRootDirectoryEntries; i++)
    {
        s_FAT12Data.DirectoryEntries.at(i) = { 0 };
    }
}

void Fat12::SetFATEntry(uint32_t fatEntry, uint16_t value)
{
    s_FAT12Data.FATTable.at(fatEntry) = value & 0x0FFF;
}

uint16_t Fat12::GetFATEntry(uint32_t fatEntry)
{
    return (s_FAT12Data.FATTable.at(fatEntry) & 0x0FFF);
}

uint32_t Fat12::FindFirstFreeFATEntry()
{
    for (uint32_t i = 0; i < (uint32_t)s_FAT12Data.FATTable.size(); i++)
    {
        if (GetFATEntry(i) == 0)
            return i;
    }

    return 0;
}

void Fat12::StoreToImage()
{
    // Write bootblock    
    m_DiskMedia->WriteToSector(0, (const char*)&s_BPBData, sizeof(s_BPBData), 3);

    // Write FAT tables

    std::vector<uint8_t> fatTable;

    for (uint32_t fatLocation : s_FAT12Data.FATLocations)
    {

        uint8_t lowValue = 0;
        uint8_t midValue = 0;
        uint8_t highValue = 0;

        for (uint32_t i = 0; i < (uint32_t)s_FAT12Data.FATTable.size(); i++)
        {
            uint16_t value = s_FAT12Data.FATTable.at(i);

            if (i & 1)
            {
                midValue += (uint8_t)((value & 0xF00) >> 8);
                lowValue += (uint8_t)value & 0xFF;

                fatTable.push_back(highValue);
                fatTable.push_back(midValue);
                fatTable.push_back(lowValue);

                lowValue = 0;
                midValue = 0;
                highValue = 0;
            }
            else
            {
                highValue += (uint8_t)((value & 0xFF0) >> 4);
                midValue += (uint8_t)((value & 0xF) << 4);
            }
        }

        uint32_t fatSectors = ((uint32_t)fatTable.size() / s_BPBData.BytesPerSector) + 1;

        for (uint32_t i = 0; i < fatSectors; i++)
        {
            m_DiskMedia->WriteToSector(fatLocation + i, (const char*)fatTable.data() + i * s_BPBData.BytesPerSector, s_BPBData.BytesPerSector);
        }
    }

    // Write root directory
    uint32_t rootDirectorySectors = ((uint32_t)s_FAT12Data.DirectoryEntries.size() * sizeof(FAT12DirectoryEntry)) / s_BPBData.BytesPerSector;

    std::cout << "Root directory sectors: " << rootDirectorySectors << "\n";
    std::cout << "Byte align: " << (s_FAT12Data.DirectoryEntries.size() * sizeof(FAT12DirectoryEntry)) % s_BPBData.BytesPerSector << "\n";

    for (uint32_t i = 0; i < rootDirectorySectors; i++)
    {
        m_DiskMedia->WriteToSector(s_FAT12Data.RootDirectoryLocation + i, (const char*)(s_FAT12Data.DirectoryEntries.data() + i * s_BPBData.BytesPerSector / sizeof(FAT12DirectoryEntry)), s_BPBData.BytesPerSector);
    }

}
