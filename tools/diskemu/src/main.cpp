#include "media/disk.h"
#include "fs/filesystem.h"

#include "json/jsonparser.h"

#include <iostream>



struct CommandLineOptions
{
	bool JSONConfig = false;
	std::string ConfigFilename;
};


struct AppData
{
	FilesystemType FSType = FilesystemType::FAT12;

	std::string Bootloader = "";
	std::string Label = "";

	std::vector<std::string> FileList;
};

static AppData s_AppData;


FilesystemType StringToFilesystemType(const std::string& str)
{
	if (str == "fat12") return FilesystemType::FAT12;
	if (str == "fat16") return FilesystemType::FAT16;
	if (str == "fat32") return FilesystemType::FAT32;

	return FilesystemType::FAT12;
}

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

				case 'b':
				{
					if (++i < argc)
						s_AppData.Bootloader = argv[i];

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
				s_AppData.FSType = StringToFilesystemType(attributeIterator.second[0]);
			}

			if (attribute == "boot" && attributeIterator.second[0] != "null")
			{
				std::cout << "Bootblock: " << attributeIterator.second[0] << "\n";

				if (!s_AppData.Bootloader.empty())
					std::cout << "Overriding boot option, " << s_AppData.Bootloader << " is replaced by " << attributeIterator.second[0] << "\n";

				s_AppData.Bootloader = attributeIterator.second[0];
			}

			if (attribute == "label")
			{
				std::cout << "Label: " << attributeIterator.second[0] << "\n";

				s_AppData.Label = attributeIterator.second[0];
			}

			if (attribute == "files")
			{
				std::cout << "Files\n";

				for (auto& file : attributeIterator.second)
				{
					std::cout << " " << file << "\n";

					s_AppData.FileList.push_back(file);
				}
			}
		}
	}


	std::shared_ptr<DiskMedia> dm = DiskMedia::Create(DiskMediaType::HD_1440);
	std::shared_ptr<Filesystem> fs = Filesystem::Create(dm, s_AppData.FSType);

	for (auto& file : s_AppData.FileList)
	{
		fs->AddFile(file);
	}

	if (!s_AppData.Label.empty())
		fs->SetLabel(s_AppData.Label);

	if (!s_AppData.Bootloader.empty())
		fs->AddBootsector(s_AppData.Bootloader);

	fs->StoreToImage();

	dm->SaveImage("test.img");

	fs->PrintDirectory();
}
