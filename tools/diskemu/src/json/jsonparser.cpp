#include "jsonparser.h"

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

	std::cout << "Parsing JSON:\n";

	size_t fileLength;

	fs.seekg(0, fs.end);
	fileLength = fs.tellg();
	fs.seekg(0, fs.beg);

	std::vector<char> fileBuffer(fileLength);
	fs.read(fileBuffer.data(), fileLength);

	fs.close();

	std::stringstream ss(fileBuffer.data());

	std::cout << "File length: " << fileLength << " bytes\n";
	std::cout << ss.str() << "\n";

	return importResult;
}
