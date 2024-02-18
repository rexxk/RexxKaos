#pragma once

#include "fs/filesystem.h"


class Fat12 : public Filesystem
{
public:
    Fat12(std::shared_ptr<DiskMedia> diskMedia);


    virtual void CreateFilesystem() override;
    virtual void StoreToImage() override;

private:
    void CalculateFATData();
    void ClearDirectory(uint32_t sector);

    void SetFATEntry(uint32_t fatEntry, uint16_t value);
    uint16_t GetFATEntry(uint32_t fatEntry);

    uint32_t FindFirstFreeFATEntry();

private:
    std::shared_ptr<DiskMedia> m_DiskMedia;
};
