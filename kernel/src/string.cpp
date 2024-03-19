#include "string.h"



const char* IntegerToString(char* buffer, uint32_t value, uint8_t base)
{
    char tempBuffer[32] = {0};

    uint32_t position = 0;

    while (value != 0)
    {
        tempBuffer[position++] = (value % base) + 0x30;
        value /= base;
    }

    uint32_t bufferPosition = 0;

    for (uint32_t i = position; i > 0; i--)
    {
        buffer[bufferPosition++] = tempBuffer[i - 1];
    }

    return buffer;
}


const char* HexToString(char* buffer, uint64_t value, uint32_t length)
{
    char tempBuffer[64] = {0};

    uint32_t position = 0;

    while (value != 0)
    {
        uint32_t hexValue = (value % 16);

        if (hexValue > 9) hexValue += 0x37;
        else hexValue += 0x30;

        tempBuffer[position++] = hexValue;

        value /= 16;
    }

    if (position > length)
        length = position;

    while (position < length)
        tempBuffer[position++] = '0';

    uint32_t bufferPosition = 0;
    buffer[bufferPosition++] = '0';
    buffer[bufferPosition++] = 'x';

    for (uint32_t i = length; i > 0; i--)
    {
        buffer[bufferPosition++] = tempBuffer[i - 1];
    }

    return buffer;
}
