#include "media/disk.h"
#include "fs/filesystem.h"

#include "json/jsonparser.h"

#include <iostream>



struct CommandLineOptions
{
	bool JSONConfig = false;
	std::string ConfigFilename;
};



int main(int argc, char** argv)
{
	CommandLineOptions options = {};

	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			char c = argv[i][1];

			switch (c)
			{
				case 'c':
				{
					options.JSONConfig = true;
				
					if (++i < argc)
						options.ConfigFilename = argv[i];
				
					break;
				}

				case 'v': case 'V':
				{
					std::cout << "DiskEMU version 1.0 by David Andersson 2024\n";
					break;
				}
			}
		}
	}

	if (options.JSONConfig == true)
	{
		JSONImportResult result = JSONParser::ParseFile(options.ConfigFilename);

		std::cout << "JSON import results:\n";
			
		for (auto& attributeIterator : result.Attributes)
		{
			std::string attribute = attributeIterator.first;

			if (attribute == "filesystem")
			{
				std::cout << "Filesystem: " << attributeIterator.second[0] << "\n";
			}

			if (attribute == "boot" && attributeIterator.second[0] != "null")
			{
				std::cout << "Bootblock: " << attributeIterator.second[0] << "\n";
			}

			if (attribute == "label")
			{
				std::cout << "Label: " << attributeIterator.second[0] << "\n";
			}

			if (attribute == "files")
			{
				std::cout << "Files\n";

				for (auto& file : attributeIterator.second)
				{
					std::cout << " " << file << "\n";
				}
			}
		}
	}


//	DiskMedia::MediaDescriptor descriptor = {};
//	descriptor.Cylinders = 80;
//	descriptor.Heads = 2;
//	descriptor.Sectors = 9;
//	descriptor.SectorSize = 512;
//
//	std::shared_ptr<DiskMedia> diskMedia = DiskMedia::Create(descriptor);
//	DiskMedia dm(DiskMediaType::DD_720);
//	std::shared_ptr<DiskMedia> dm = DiskMedia::Create(DiskMediaType::DD_880);
	std::shared_ptr<DiskMedia> dm = DiskMedia::Create(DiskMediaType::HD_1440);

	dm->ReadSector(31);

	dm->ReadSector(5, 1, 1);

	std::shared_ptr<Filesystem> fat12 = Filesystem::Create(dm, FilesystemType::FAT12);

	fat12->AddFile("hello.txt");
	fat12->AddFile("Tools/DiskEMU/Makefile");
	fat12->AddFile("LICENSE");
	fat12->SetLabel("REXXOS");
	fat12->StoreToImage();

	dm->SaveImage("test.img");

	fat12->PrintDirectory();
}
