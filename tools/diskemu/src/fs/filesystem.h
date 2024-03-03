#pragma once

#include "media/disk.h"

#include <string>
#include <memory>

enum class FilesystemType
{
    FAT12,
    FAT16,
    FAT32,
};


class Filesystem
{
public:
    struct Metrics
    {
        uint32_t TotalSectorCount = 0;
        uint32_t UsedSectorCount = 0;
    };

public:
    static std::shared_ptr<Filesystem> Create(std::shared_ptr<DiskMedia> diskMedia, FilesystemType fsType);
    static std::shared_ptr<Filesystem> Open(std::shared_ptr<DiskMedia> diskMedia);
    virtual ~Filesystem() = default;

    virtual void ReadFilesystem(std::shared_ptr<DiskMedia> diskMedia) = 0;

    virtual void AddFile(const std::string& filename) = 0;
    virtual void RemoveFile(const std::string& filename) = 0;

    virtual void AddBootsector(const std::string& filename) = 0;

    virtual void SetLabel(const std::string& label) = 0;

    virtual void CreateFilesystem() = 0;
    virtual void StoreToImage() = 0;

    virtual uint32_t GetFreeSectorCount() = 0;

    virtual void PrintDirectory() = 0;
};
