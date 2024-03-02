#pragma once

#include <string>
#include <map>
#include <vector>


struct JSONImportResult
{
	bool HasBootloader = false;
	std::string BootloaderFilename = "";

	std::string Label = "default";

	std::vector<std::string> Filenames;
};


class JSONParser
{
public:


	struct BlockData
	{
		std::vector<std::map<std::string, std::string>> Values;
	};

	struct Block
	{
		BlockData Data;

		std::vector<Block> Blocks;
	};

public:
	static JSONImportResult ParseFile(const std::string& path);

};
