#pragma once

#include "fs/filesystem.h"


class Fat12 : public Filesystem
{
public:
    Fat12(std::shared_ptr<DiskMedia> diskMedia);


    virtual void CreateFilesystem() override;

private:
    std::shared_ptr<DiskMedia> m_DiskMedia;
};
