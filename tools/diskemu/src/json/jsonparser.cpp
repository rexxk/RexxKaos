#include "jsonparser.h"

#include "utils/StringTools.h"

#include <iostream>
#include <fstream>
#include <sstream>

JSONImportResult JSONParser::ParseFile(const std::string& path)
{
	JSONImportResult importResult = {};

	std::ifstream fs(path, std::ios::in | std::ios::binary);

	if (!fs.is_open())
	{
		std::cout << "JSONParser: Unable to open file " << path << "\n";
		return importResult;
	}

//	std::cout << "Parsing JSON:\n";

	size_t fileLength;

	fs.seekg(0, fs.end);
	fileLength = fs.tellg();
	fs.seekg(0, fs.beg);

	std::vector<char> fileBuffer(fileLength);
	fs.read(fileBuffer.data(), fileLength);

	fs.close();

	std::stringstream ss(fileBuffer.data());

//	std::cout << "File length: " << fileLength << " bytes\n";

	while (!ss.eof())
	{
		char c = 0;
		ss >> c;

		if (c == '{')
		{
			Block block = ReadBlock(ss);

			for (auto& attribute : block.Attributes)
			{
				importResult.Attributes[attribute.first] = attribute.second;
			}
		}
	}

//	std::cout << ss.str() << "\n";

	return importResult;
}

JSONParser::Block JSONParser::ReadBlock(std::stringstream& ss)
{
	Block newBlock;

	char c = 0;
	ss >> c;

	while (c != '}' && !ss.eof())
	{
		if (c == '\"')
		{
			auto [attribute, values] = ReadValue(ss);

			newBlock.Attributes[attribute] = values;
		}

		ss >> c;
	}

	return newBlock;
}

JSONParser::BlockValue JSONParser::ReadValue(std::stringstream& ss)
{	
	std::string valueName = "";
	std::string value = "";

	valueName = GetValueString(ss);
	value = GetValueString(ss);

	std::vector<std::string> values;

	if (value == "[")
	{
		while (true)
		{
			value = GetValueString(ss);

			if (value == "]")
				break;
			else 
				values.push_back(value);
		}
	}
	else
	{
		values.push_back(value);
	}

	return std::make_pair(valueName, values);
}

std::string JSONParser::GetValueString(std::stringstream& ss)
{
	std::string value;
	ss >> value;

	if (value == "[" || value == "]")
		return value;
	
	return CleanJSONString(value);
}
