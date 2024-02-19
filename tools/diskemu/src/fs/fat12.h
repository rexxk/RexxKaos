#pragma once

#include "fs/filesystem.h"


class Fat12 : public Filesystem
{
public:
    Fat12(std::shared_ptr<DiskMedia> diskMedia);

    virtual void AddFile(const std::string& filename) override;
    virtual void RemoveFile(const std::string& filename) override;

    virtual void CreateFilesystem() override;
    virtual void StoreToImage() override;

private:
    void CalculateFATData();
    void ClearDirectory(uint32_t sector);

    void SetFATEntry(uint32_t fatEntry, uint16_t value);
    uint16_t GetFATEntry(uint32_t fatEntry);

    uint32_t FindFirstFreeFATEntry();
    uint32_t FindRootDirectoryEntry();

private:
    std::shared_ptr<DiskMedia> m_DiskMedia;
};
