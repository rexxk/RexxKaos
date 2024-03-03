#include "StringTools.h"

#include <cstdint>
#include <cstring>


void GetFAT12Filename(const std::string& path, char* filename)
{
    std::memset(filename, ' ', 11);

    size_t maxNameLength = 8;
    size_t maxExtensionLength = 3;

    size_t filenameStart = path.find_last_of("/\\");
    size_t extensionStart = path.find_last_of('.');

    if (filenameStart == std::string::npos)
        filenameStart = 0;
    else
        filenameStart++;

    size_t fileLength = 0;

    if (extensionStart != std::string::npos)
    {
        fileLength = extensionStart - filenameStart;
    }
    else
    {
        fileLength = path.length() - filenameStart;
    }

    if (fileLength > 8)
        fileLength = 8;

    std::memcpy(filename, path.data() + filenameStart, fileLength);

    if (extensionStart == std::string::npos)
        return;
    else
    {
        size_t extensionLength = path.length() - (extensionStart + 1);
        std::memcpy(filename + maxNameLength, path.data() + extensionStart + 1, extensionLength);
    }

    for (uint32_t i = 0; i < 11; i++)
    {
        filename[i] = std::toupper(filename[i]);
    }

}

std::string CleanJSONString(const std::string& str)
{
    std::string cleanedString = "";

    for (size_t i = 0; i < str.size(); i++)
    {
        if (std::isalnum(str[i]) || str[i] == '.' || str[i] == '/' || str[i] == '\\')
            cleanedString += str[i];
    }

    return cleanedString;
}
