#include "fat12.h"

#include <iostream>
#include <cstring>

#ifdef _WIN32
#pragma pack(push, 1)
struct FilesystemData
#endif

#ifdef __linux__
struct __attribute__((packed)) FilesystemData
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

static FilesystemData s_FSData;


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
        s_FSData.TotalSectors = 0;
        s_FSData.TotalSectorsLarge = totalSectorCount;
    }
    else
    {
        s_FSData.TotalSectors = totalSectorCount;
        s_FSData.TotalSectorsLarge = 0;
    }

    std::memcpy(s_FSData.OEMName, "REXXKAOS", 8);
    s_FSData.BytesPerSector = mediaDescriptor.SectorSize;
    s_FSData.SectorsPerCluster = 1;
    s_FSData.ReservedSectorsCount = 1;
    s_FSData.NumberOfFATs = 2;
    s_FSData.MaxRootDirectoryEntries = 224;
    s_FSData.MediaDescriptor = 0xF0;
    s_FSData.SectorsPerFAT = 9;
    s_FSData.SectorsPerTrack = mediaDescriptor.Sectors;
    s_FSData.NumberOfHeads = mediaDescriptor.Heads;
    s_FSData.HiddenSectors = 0;

    s_FSData.DriveNumber = 0;
    s_FSData.FlagsWindowsNT = 0;
    s_FSData.BootSignature = 0x29;
    s_FSData.VolumeID = 0xDEADBEEF;
    std::memcpy(s_FSData.VolumeLabel, "REXXKAOS   ", 11);
    std::memcpy(s_FSData.FileSystemType, "FAT12   ", 8);

    std::cout << "Size of boot parameter block: " << sizeof(s_FSData) << " bytes\n";

    m_DiskMedia->WriteToSector(0, (const char*)&s_FSData, sizeof(s_FSData), 3);
}
