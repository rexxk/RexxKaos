#include "fs/filesystem.h"
#include "media/disk.h"

#include "fs/fat12.h"


std::shared_ptr<Filesystem> Filesystem::Create(std::shared_ptr<DiskMedia> diskMedia, FilesystemType fsType)
{
    switch (fsType)
    {
        case FilesystemType::FAT12: return std::make_shared<Fat12>(diskMedia);
        case FilesystemType::FAT16: return nullptr;
        case FilesystemType::FAT32: return nullptr;
    }

    return nullptr;
}
