#include "media/disk.h"
#include "fs/filesystem.h"

#include <iostream>






int main()
{
	DiskMedia::MediaDescriptor descriptor = {};
	descriptor.Cylinders = 80;
	descriptor.Heads = 2;
	descriptor.Sectors = 9;
	descriptor.SectorSize = 512;

	std::shared_ptr<DiskMedia> diskMedia = DiskMedia::Create(descriptor);
//	DiskMedia dm(DiskMediaType::DD_720);
//	std::shared_ptr<DiskMedia> dm = DiskMedia::Create(DiskMediaType::DD_880);
	std::shared_ptr<DiskMedia> dm = DiskMedia::Create(DiskMediaType::HD_1440);

	dm->ReadSector(31);

	dm->ReadSector(5, 1, 1);

	std::shared_ptr<Filesystem> fat12 = Filesystem::Create(dm, FilesystemType::FAT12);

	dm->SaveImage("test.img");
}
