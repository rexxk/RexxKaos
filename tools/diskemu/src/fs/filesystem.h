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
    static std::shared_ptr<Filesystem> Create(std::shared_ptr<DiskMedia> diskMedia, FilesystemType fsType);

    virtual ~Filesystem() = default;

    virtual void AddFile(const std::string& filename) = 0;
    virtual void RemoveFile(const std::string& filename) = 0;

    virtual void CreateFilesystem() = 0;
    virtual void StoreToImage() = 0;
};
