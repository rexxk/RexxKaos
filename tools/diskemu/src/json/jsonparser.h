#pragma once

#include <string>
#include <map>
#include <vector>
#include <unordered_map>

#include <sstream>

struct JSONImportResult
{
	bool HasBootloader = false;
	std::string BootloaderFilename = "";

	std::string Label = "default";

	std::vector<std::string> Filenames;

	std::unordered_map<std::string, std::vector<std::string>> Attributes;
};


class JSONParser
{
public:
	using BlockValue = std::pair<std::string, std::vector<std::string>>;

	struct Block
	{
		std::unordered_map<std::string, std::vector<std::string>> Attributes;

		std::vector<Block> Blocks;
	};

public:
	static JSONImportResult ParseFile(const std::string& path);

private:
	static Block ReadBlock(std::stringstream& ss);
	static BlockValue ReadValue(std::stringstream& ss);

	static std::string GetValueString(std::stringstream& ss);
};
