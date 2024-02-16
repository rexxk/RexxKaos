#include "media/disk.h"

#include <iostream>






int main()
{
	DiskMedia::MediaDescriptor descriptor = {};
	descriptor.Cylinders = 80;
	descriptor.Heads = 2;
	descriptor.Sectors = 9;
	descriptor.SectorSize = 512;

	DiskMedia diskMedia(descriptor);
	DiskMedia dm(DiskMediaType::DD_720);
	DiskMedia dm2(DiskMediaType::DD_880);
	DiskMedia dm3(DiskMediaType::HD_1440);

	dm.ReadSector(31);
	dm2.ReadSector(31);
	dm3.ReadSector(31);

	dm.ReadSector(5, 1, 1);
	dm2.ReadSector(10, 1, 0);
	dm3.ReadSector(14, 0, 1);
}
