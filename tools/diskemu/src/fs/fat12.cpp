#include "fat12.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include <filesystem>

#include <vector>

#include "utils/StringTools.h"



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


void Fat12::PrintDirectory()
{
    std::cout << "Directory reading\n"
        << "-----------------\n";

    uint32_t totalFileSize = 0;

    for (FAT12DirectoryEntry& entry : s_FAT12Data.DirectoryEntries)
    {
        if (entry.Filename[0] == 0 || (entry.Attributes & (uint32_t)FAT12DirectoryFlag::VolumeLabel))
            continue;

        for (uint32_t i = 0; i < 8; i++)
            std::cout << entry.Filename[i];

        std::cout << ".";

        for (uint32_t i = 0; i < 3; i++)
            std::cout << entry.Filename[i + 8];

        std::cout << "  " << entry.FileSize << " bytes\n";

        totalFileSize += entry.FileSize;
    }

    std::cout << "\nTotal file size: " << totalFileSize << "\n";
    std::cout << "Free size : " << (m_Metrics.TotalSectorCount - m_Metrics.UsedSectorCount) * s_BPBData.BytesPerSector << " bytes\n";
    std::cout << "Total size: " << m_Metrics.TotalSectorCount * s_BPBData.BytesPerSector << " bytes\n";
}


Fat12::Fat12(std::shared_ptr<DiskMedia> diskMedia)
    : m_DiskMedia(diskMedia)
{
    CreateFilesystem();
}

void Fat12::ReadFilesystem(std::shared_ptr<DiskMedia> diskMedia)
{

}

void Fat12::CreateFilesystem()
{
    std::cout << "FAT12 filesystem initialization\n";

    DiskMedia::MediaDescriptor mediaDescriptor = m_DiskMedia->GetMediaDescriptor();

    uint32_t totalSectorCount = m_DiskMedia->GetTotalSectorCount();

    m_Metrics.TotalSectorCount = totalSectorCount;

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

    CalculateFATData();
}

void Fat12::CalculateFATData()
{
    uint16_t location = s_BPBData.ReservedSectorsCount;

    for (uint32_t i = 0; i < s_BPBData.NumberOfFATs; i++)
    {
        s_FAT12Data.FATLocations.push_back(location);
        location += s_BPBData.SectorsPerFAT;
    }

    s_FAT12Data.RootDirectoryLocation = location;
    s_FAT12Data.DataRegionStart = (s_FAT12Data.RootDirectoryLocation - 2) + (s_BPBData.MaxRootDirectoryEntries * sizeof(FAT12DirectoryEntry)) / s_BPBData.BytesPerSector;

    m_Metrics.UsedSectorCount = s_FAT12Data.DataRegionStart + 2;

    if (s_BPBData.TotalSectors == 0)
        s_FAT12Data.TotalSectorCount = s_BPBData.TotalSectorsLarge;
    else
        s_FAT12Data.TotalSectorCount = s_BPBData.TotalSectors;

    s_FAT12Data.FATEntries = (uint32_t)(s_BPBData.SectorsPerFAT * s_BPBData.BytesPerSector * 1.5);

    s_FAT12Data.FATTable.resize(s_FAT12Data.FATEntries);

    SetFATEntry(0, (0xF << 8) + s_BPBData.MediaDescriptor);
    SetFATEntry(1, 0xFFF);

    s_FAT12Data.DirectoryEntries.resize(s_BPBData.MaxRootDirectoryEntries);
    ClearDirectory(s_FAT12Data.RootDirectoryLocation);
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

uint32_t Fat12::FindFirstFreeFATEntry(uint32_t offset)
{
    for (uint32_t i = offset; i < (uint32_t)s_FAT12Data.FATTable.size(); i++)
    {
        if (GetFATEntry(i) == 0)
        {
            SetFATEntry(i, 0xFFF);
            return i;
        }
    }

    return 0;
}

void Fat12::StoreToImage()
{
    // Write bootblock    
    m_DiskMedia->WriteToSector(0, (const char*)&s_BPBData, sizeof(s_BPBData), 3);

    // Write FAT tables

    std::vector<uint8_t> fatTable;

    uint32_t fatPair = 0;

    for (uint32_t i = 0; i < (uint32_t)s_FAT12Data.FATTable.size(); i++)
    {
        
        uint16_t value = s_FAT12Data.FATTable[i];

        if (i & 1)
        {
            fatPair += (value & 0x0FFF) << 12;

            uint8_t lowValue = (uint8_t)(fatPair & 0xFF);
            uint8_t midValue = (uint8_t)(fatPair >> 8) & 0xFF;
            uint8_t highValue = (uint8_t)(fatPair >> 16) & 0xFF;

            fatTable.push_back(lowValue);
            fatTable.push_back(midValue);
            fatTable.push_back(highValue);

            fatPair = 0;
        }
        else
        {
            fatPair += (value & 0x0FFF);
        }
    }


    uint32_t fatSectors = ((uint32_t)fatTable.size() / s_BPBData.BytesPerSector) + 1;

    for (uint32_t fatLocation : s_FAT12Data.FATLocations)
    {
        for (uint32_t i = 0; i < fatSectors; i++)
        {
            m_DiskMedia->WriteToSector(fatLocation + i, (const char*)fatTable.data() + i * s_BPBData.BytesPerSector, s_BPBData.BytesPerSector);
        }

    }

    WriteRootDirectory();
}

void Fat12::WriteRootDirectory()
{
    uint32_t rootDirectorySectors = ((uint32_t)s_FAT12Data.DirectoryEntries.size() * sizeof(FAT12DirectoryEntry)) / s_BPBData.BytesPerSector;

    for (uint32_t i = 0; i < rootDirectorySectors; i++)
    {
        m_DiskMedia->WriteToSector(s_FAT12Data.RootDirectoryLocation + i, (const char*)(s_FAT12Data.DirectoryEntries.data() + i * s_BPBData.BytesPerSector / sizeof(FAT12DirectoryEntry)), s_BPBData.BytesPerSector);
    }

}

void Fat12::AddFile(const std::string& filename)
{
    char filenameStr[11];
    GetFAT12Filename(filename, filenameStr);

    std::ifstream fs(filename, std::ios::in | std::ios::binary);

    if (!fs.is_open())
    {
        std::cout << "Failed to open " << filename << "\n";
        return;
    }

    fs.seekg(0, fs.end);
    uint32_t fileSize = (uint32_t)fs.tellg();
    fs.seekg(0, fs.beg);

//    std::cout << "Adding " << filename << " to disk (" << fileSize << " bytes)\n";

    std::vector<uint8_t> data;
    data.resize(fileSize);

    fs.read((char*)data.data(), fileSize);

    fs.close();

    uint32_t neededSectors = (fileSize / s_BPBData.BytesPerSector) + 1;

    if (neededSectors > (m_Metrics.TotalSectorCount - m_Metrics.UsedSectorCount))
    {
        std::cout << "File " << filename << " is too large for the medium.\n";
        return;
    }

//    std::cout << "Needed sectors on disk: " << neededSectors << "\n";

    // Write directory entry
    uint32_t directoryEntry = FindRootDirectoryEntry();
    FAT12DirectoryEntry& entry = s_FAT12Data.DirectoryEntries[directoryEntry];
    std::memcpy(entry.Filename, filenameStr, 11);
    entry.FileSize = fileSize;
    entry.Attributes = (uint32_t)FAT12DirectoryFlag::Archive;
    entry.FirstCluster = FindFirstFreeFATEntry(2);

//    std::cout << "First cluster: " << entry.FirstCluster << "\n";
    // Add data...
//    std::cout << "Data: " << data.data() << "\n";

    uint32_t dataRemaining = (uint32_t)data.size();
//    uint32_t nextCluster = FindFirstFreeFATEntry(entry.FirstCluster); 
    uint32_t nextCluster = entry.FirstCluster;

    uint32_t dataIndex = 0;

    while (dataRemaining != 0)
    {
        // Write data to data sector

//        std::cout << "FAT address: " << nextCluster << "\n";

        m_DiskMedia->WriteToSector(s_FAT12Data.DataRegionStart + nextCluster, (const char*)data.data() + (dataIndex++ * s_BPBData.BytesPerSector), 
            dataRemaining > s_BPBData.BytesPerSector ? s_BPBData.BytesPerSector : dataRemaining);

        m_Metrics.UsedSectorCount++;

        if (dataRemaining > s_BPBData.BytesPerSector)
        {
            dataRemaining -= s_BPBData.BytesPerSector;
            uint32_t newCluster = FindFirstFreeFATEntry(nextCluster);
            SetFATEntry(nextCluster, newCluster);
            nextCluster = newCluster;
        }
        else
        {
            dataRemaining = 0;
            SetFATEntry(nextCluster, 0xFFF);
        }
    }

}

void Fat12::AddBootsector(const std::string& filename)
{
    std::ifstream fs(filename, std::ios::in | std::ios::binary);

    if (!fs.is_open())
    {
        std::cout << "Unable to open bootsector file " << filename << "\n";
        return;
    }

    size_t fileSize;

    fs.seekg(0, fs.end);
    fileSize = fs.tellg();
    fs.seekg(0, fs.beg);

    std::vector<uint8_t> fileData(fileSize);
    fs.read((char*)fileData.data(), fileSize);

    fs.close();

    if (fileSize != 512)
    {
        std::cout << "Invalid bootsector filesize. Should be 512 bytes, but are " << fileSize << " bytes.\n";
        return;
    }

    if (!(fileData[510] == 0x55 && fileData[511] == 0xAA))
    {
        std::cout << "Invalid bootsector signature.\n";
        return;
    }

    m_DiskMedia->WriteToSector(0, (const char*)fileData.data(), s_BPBData.BytesPerSector, 0);
}

void Fat12::SetLabel(const std::string& label)
{
    char filenameStr[11];
    GetFAT12Filename(label, filenameStr);

    uint32_t directoryEntry = FindRootDirectoryEntry();
    FAT12DirectoryEntry& entry = s_FAT12Data.DirectoryEntries[directoryEntry];
    std::memcpy(entry.Filename, filenameStr, 11);
    entry.Attributes = (uint32_t)FAT12DirectoryFlag::VolumeLabel;
    entry.FirstCluster = 0;
}

void Fat12::RemoveFile(const std::string& filename)
{

}

uint32_t Fat12::FindRootDirectoryEntry()
{
    for (uint32_t i = 0; i < (uint32_t)s_FAT12Data.DirectoryEntries.size(); i++)
    {
        if (s_FAT12Data.DirectoryEntries[i].Filename[0] == 0)
            return i;
    }

    return -1;
}

uint32_t Fat12::GetFreeSectorCount()
{
    return (m_Metrics.TotalSectorCount - m_Metrics.UsedSectorCount);
}
